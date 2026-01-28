#ifndef DIGITAL_TWIN_PRIOR_H
#define DIGITAL_TWIN_PRIOR_H

#include <vector>
#include <string>
#include <Eigen/Core>
#include <yaml-cpp/yaml.h>

// 간단한 digital twin prior 타입 정의
struct ObjectPrior {
    int id = 0;
    Eigen::Vector3d dims = Eigen::Vector3d::Zero(); // cuboid: L,W,H ; cylinder: radius,height,unused
    Eigen::Vector3d pos = Eigen::Vector3d::Zero();  // x,y,z
    Eigen::Vector3d rpy = Eigen::Vector3d::Zero();  // roll,pitch,yaw (rad)
    enum Type { CUBOID=0, CYLINDER=1 } type = CUBOID;
};

// YAML 파일 (config/digital_twin.yaml) 파싱기
static inline bool loadPriorsFromYaml(const std::string &yaml_path, std::vector<ObjectPrior> &out) {
    try {
        YAML::Node root = YAML::LoadFile(yaml_path);
        if (!root["priors"]) return true; // no priors, graceful
        for (const auto &n : root["priors"]) {
            ObjectPrior p;
            if (n["id"]) p.id = n["id"].as<int>();
            if (n["dims"]) {
                auto dims = n["dims"];
                for (int i=0;i<3 && i< (int)dims.size(); ++i) p.dims[i] = dims[i].as<double>();
            }
            if (n["pose"]) {
                auto pose = n["pose"];
                for (int i=0;i<6 && i<(int)pose.size(); ++i) {
                    if (i<3) p.pos[i] = pose[i].as<double>();
                    else p.rpy[i-3] = pose[i].as<double>();
                }
            }
            if (n["type"]) {
                std::string t = n["type"].as<std::string>();
                if (t == "cylinder") p.type = ObjectPrior::CYLINDER;
                else p.type = ObjectPrior::CUBOID;
            }
            out.push_back(p);
        }
        return true;
    } catch (const std::exception &e) {
        // 실패시 로그는 호출측에서 처리
        return false;
    }
}

// 점이 prior 내부에 있는지 검사하는 유틸 (cuboid는 axis-aligned in prior frame, cylinder는 z축을 기준으로)
static inline bool pointInPrior(const Eigen::Vector3d &pt_world, const ObjectPrior &p) {
    // transform point to object frame (translate then rotate inverse)
    // build rotation from rpy
    double cr = cos(p.rpy[0]), sr = sin(p.rpy[0]);
    double cp = cos(p.rpy[1]), sp = sin(p.rpy[1]);
    double cy = cos(p.rpy[2]), sy = sin(p.rpy[2]);
    Eigen::Matrix3d R;
    // ZYX (yaw-pitch-roll) rotation
    R = Eigen::AngleAxisd(p.rpy[2], Eigen::Vector3d::UnitZ()).toRotationMatrix()
      * Eigen::AngleAxisd(p.rpy[1], Eigen::Vector3d::UnitY()).toRotationMatrix()
      * Eigen::AngleAxisd(p.rpy[0], Eigen::Vector3d::UnitX()).toRotationMatrix();

    Eigen::Vector3d local = R.transpose() * (pt_world - p.pos);

    if (p.type == ObjectPrior::CUBOID) {
        Eigen::Vector3d half = p.dims * 0.5;
        return (std::abs(local[0]) <= half[0] && std::abs(local[1]) <= half[1] && std::abs(local[2]) <= half[2]);
    } else { // CYLINDER: dims[0]=radius, dims[1]=height
        double r = p.dims[0];
        double h = p.dims[1];
        double z = local[2];
        double rr = local[0]*local[0] + local[1]*local[1];
        return (rr <= r*r && std::abs(z) <= h*0.5);
    }
}

#endif // DIGITAL_TWIN_PRIOR_H