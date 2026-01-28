# Stage A: Camera Calibration Parameters

## 개요 (Overview)

Stage A는 디지털 트윈 시스템의 초기 캘리브레이션 단계로, 카메라 모델 및 외부 파라미터를 고정합니다.

Stage A is the initial calibration phase of the digital twin system, which fixes the camera model and extrinsic parameters.

## 입력 (Input)

- 센서 구성 (Sensor configuration)
- 마커(피듀셜) 설계/배치 (Marker/fiducial design and placement)
- Digital Twin Prior Θ0

## 출력 (Output)

- 월드 좌표계 {W} (World coordinate frame {W})
- 캘리브레이션 산출물 (Calibration deliverables):
  - 카메라 내부 파라미터 (Camera intrinsics)
  - 카메라 외부 파라미터 (Camera extrinsics)
  - 태그/마커 위치 정보 (Tag/marker position information)

## 구성 방법 (Configuration)

### 1. 카메라 내부 파라미터 (Camera Intrinsics)

카메라 캘리브레이션으로부터 얻은 내부 파라미터를 설정합니다.

Set the intrinsic parameters obtained from camera calibration.

```yaml
camera:
  intrinsics:
    fx: 700.0      # 초점 거리 x (Focal length x) in pixels
    fy: 700.0      # 초점 거리 y (Focal length y) in pixels
    cx: 640.0      # 주점 x (Principal point x) in pixels
    cy: 360.0      # 주점 y (Principal point y) in pixels
    k1: 0.0        # 방사 왜곡 계수 (Radial distortion coefficient)
    k2: 0.0        # 방사 왜곡 계수 (Radial distortion coefficient)
    k3: 0.0        # 방사 왜곡 계수 (Radial distortion coefficient)
    p1: 0.0        # 접선 왜곡 계수 (Tangential distortion coefficient)
    p2: 0.0        # 접선 왜곡 계수 (Tangential distortion coefficient)
```

**참고 (Note):** 이 파라미터들은 Stage A 캘리브레이션에서 고정되며, 런타임에는 변경되지 않습니다.
These parameters are fixed from Stage A calibration and do not change during runtime.

### 2. 카메라 외부 파라미터 (Camera Extrinsics)

카메라에서 LiDAR/IMU로의 변환을 설정합니다.

Set the transformation from camera to LiDAR/IMU.

```yaml
camera:
  extrinsics:
    camera_to_lidar_T: [ 0.0, 0.0, 0.0 ]  # 평행이동 [x, y, z] in meters
    camera_to_lidar_R: [ 1., 0., 0.,      # 회전 행렬 (Rotation matrix, row-major)
                        0., 1., 0.,
                        0., 0., 1.]
```

### 3. 태그/마커 파라미터 (Tag/Marker Parameters) - 임시값

태그 관련 임시값을 설정합니다. 나중에 Tag-Plane으로 교체될 예정입니다.

Set temporary tag-related values. These will be replaced with Tag-Plane later.

```yaml
tags:
  tag_size: 0.1          # 태그 크기 (Tag size) in meters
  tag_family: "tag36h11" # AprilTag 패밀리 (AprilTag family)
  
  # 월드 프레임에서의 태그 위치 (Tag positions in world frame)
  tag_positions:
    - id: 0
      position: [0.0, 0.0, 0.0]
      orientation: [1., 0., 0., 0., 1., 0., 0., 0., 1.]
    - id: 1
      position: [1.0, 0.0, 0.0]
      orientation: [1., 0., 0., 0., 1., 0., 0., 0., 1.]
    - id: 2
      position: [0.0, 1.0, 0.0]
      orientation: [1., 0., 0., 0., 1., 0., 0., 0., 1.]
```

**중요 (Important):** 태그 위치는 임시값이며, 향후 Tag-Plane 기반 시스템으로 교체될 예정입니다.
Tag positions are temporary values and will be replaced with a Tag-Plane based system in the future.

## 사용 방법 (Usage)

### 런타임 로그 확인 (Check Runtime Logs)

시스템 시작 시 다음과 같은 로그가 출력됩니다:

The following logs are printed at system startup:

```
[INFO] === Stage A: Camera Calibration Parameters ===
[INFO] Camera Intrinsics (fixed):
[INFO]   fx: 700.00, fy: 700.00
[INFO]   cx: 640.00, cy: 360.00
[INFO]   k1: 0.000000, k2: 0.000000, k3: 0.000000
[INFO]   p1: 0.000000, p2: 0.000000
[INFO] Camera Extrinsics (Camera to LiDAR):
[INFO]   Translation: [0.000, 0.000, 0.000]
[INFO]   Rotation matrix loaded (3x3)
[INFO] Tag Parameters (temporary, to be replaced with Tag-Plane):
[INFO]   Tag size: 0.100 m
[INFO]   Tag family: tag36h11
[INFO] ==============================================
```

### 파라미터 업데이트 (Update Parameters)

1. 해당 센서의 config 파일을 엽니다 (예: `config/avia.yaml`)
2. `camera` 및 `tags` 섹션의 값을 업데이트합니다
3. 노드를 재시작하여 새 파라미터를 로드합니다

## 다음 단계 (Next Steps)

1. **Tag-Plane 시스템 구현**: 임시 태그 위치를 Tag-Plane 기반 시스템으로 교체
2. **카메라 모델 활용**: 고정된 카메라 파라미터를 활용한 이미지 처리 파이프라인 구현
3. **검증**: 캘리브레이션 정확도 검증 및 테스트

## 참고사항 (Notes)

- 모든 설정 파일(avia.yaml, mid360.yaml, velodyne.yaml, horizon.yaml, ouster64.yaml)에 동일한 구조의 파라미터가 추가되었습니다.
- 카메라 내부 파라미터는 Stage A에서 고정되며, 실행 중에는 변경되지 않습니다.
- 태그 파라미터는 임시값으로, 향후 업데이트가 필요합니다.
