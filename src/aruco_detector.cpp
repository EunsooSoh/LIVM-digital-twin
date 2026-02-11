#include "aruco_detector.h"
#include <cv_bridge/cv_bridge.h>

ArucoDetector::ArucoDetector() : marker_length_(0.1) {
    // Dictionary 초기화 (Ptr 없이 직접 생성)
    dictionary_ = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    
    // DetectorParameters 초기화 (Ptr 없이 직접 생성)
    params_ = cv::aruco::DetectorParameters();
    
    // ArucoDetector 초기화
    detector_ = cv::aruco::ArucoDetector(dictionary_, params_);
    
    // 기본 카메라 행렬 초기화 (나중에 setCameraParams로 변경 가능)
    camera_matrix_ = Eigen::Matrix3d::Identity();
    camera_matrix_(0, 0) = 532.6182861328125; // fx
    camera_matrix_(1, 1) = 532.6182861328125;  // fy
    camera_matrix_(0, 2) = 473.020751953125;  // cx
    camera_matrix_(1, 2) = 272.11669921875;  // cy
    
    // 왜곡 계수 초기화 (왜곡 없음)
    dist_coeffs_ = Eigen::VectorXd::Zero(5);
}

void ArucoDetector::setCameraParams(const Eigen::Matrix3d& K, 
                                    const Eigen::VectorXd& distCoeffs) {
    camera_matrix_ = K;
    dist_coeffs_ = distCoeffs;
}

bool ArucoDetector::detectMarkers(
    const sensor_msgs::msg::Image::ConstSharedPtr& img,
    std::vector<int>& marker_ids,
    std::vector<Eigen::Matrix4d>& marker_poses) 
{
    // ROS Image -> OpenCV Mat 변환
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
    } catch (cv_bridge::Exception& e) {
        std::cerr << "cv_bridge exception: " << e.what() << std::endl;
        return false;
    }
    
    // ArUco 마커 감지 (새로운 API 사용)
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    std::vector<std::vector<cv::Point2f>> rejected;
    
    detector_.detectMarkers(cv_ptr->image, corners, ids, rejected);
    
    if (ids.empty()) {
        return false;
    }
    
    // 카메라 행렬을 cv::Mat으로 변환
    cv::Mat camera_mat = (cv::Mat_<double>(3, 3) << 
        camera_matrix_(0, 0), 0, camera_matrix_(0, 2),
        0, camera_matrix_(1, 1), camera_matrix_(1, 2),
        0, 0, 1);
    
    // 왜곡 계수를 cv::Mat으로 변환
    cv::Mat dist_coeffs_cv;
    if (dist_coeffs_.size() > 0) {
        dist_coeffs_cv = cv::Mat::zeros(dist_coeffs_.size(), 1, CV_64F);
        for (int i = 0; i < dist_coeffs_.size(); i++) {
            dist_coeffs_cv.at<double>(i, 0) = dist_coeffs_[i];
        }
    } else {
        dist_coeffs_cv = cv::Mat();  // 빈 Mat
    }
    
    // 포즈 추정 (OpenCV 4.7+에서는 solvePnP 사용)
    std::vector<cv::Vec3d> rvecs, tvecs;
    
    // 마커의 3D 코너 포인트 정의 (마커가 XY 평면에 있다고 가정)
    std::vector<cv::Point3f> marker_corners_3d = {
        cv::Point3f(-marker_length_ / 2.f,  marker_length_ / 2.f, 0),
        cv::Point3f( marker_length_ / 2.f,  marker_length_ / 2.f, 0),
        cv::Point3f( marker_length_ / 2.f, -marker_length_ / 2.f, 0),
        cv::Point3f(-marker_length_ / 2.f, -marker_length_ / 2.f, 0)
    };
    
    // 각 마커에 대해 포즈 추정
    for (size_t i = 0; i < corners.size(); i++) {
        cv::Vec3d rvec, tvec;
        cv::solvePnP(
            marker_corners_3d,
            corners[i],
            camera_mat,
            dist_coeffs_cv,
            rvec,
            tvec
        );
        rvecs.push_back(rvec);
        tvecs.push_back(tvec);
    }
    
    // Eigen 형식으로 변환
    marker_ids = ids;
    marker_poses.resize(ids.size());
    
    for (size_t i = 0; i < ids.size(); i++) {
        cv::Mat R;
        cv::Rodrigues(rvecs[i], R);
        
        Eigen::Matrix4d pose = Eigen::Matrix4d::Identity();
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                pose(r, c) = R.at<double>(r, c);
            }
            pose(r, 3) = tvecs[i][r];
        }
        marker_poses[i] = pose;
    }
    
    return true;
}