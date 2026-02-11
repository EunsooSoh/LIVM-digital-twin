#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/objdetect/aruco_dictionary.hpp>
#include <Eigen/Core>
#include <sensor_msgs/msg/image.hpp>

class ArucoDetector {
public:
    ArucoDetector();
    
    // ArUco marker 감지 및 포즈 추정
    bool detectMarkers(const sensor_msgs::msg::Image::ConstSharedPtr& img,
                       std::vector<int>& marker_ids,
                       std::vector<Eigen::Matrix4d>& marker_poses);
    
    void setCameraParams(const Eigen::Matrix3d& K, 
                         const Eigen::VectorXd& distCoeffs);
    
private:
    cv::aruco::Dictionary dictionary_;           
    cv::aruco::DetectorParameters params_;       
    cv::aruco::ArucoDetector detector_;         
    Eigen::Matrix3d camera_matrix_;
    Eigen::VectorXd dist_coeffs_;
    double marker_length_;
};