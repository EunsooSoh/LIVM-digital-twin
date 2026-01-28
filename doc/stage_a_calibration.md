# Stage A: Camera Calibration Parameters

## 개요 (Overview)

Stage A는 디지털 트윈 시스템의 초기 캘리브레이션 단계로, 카메라 모델 및 외부 파라미터를 고정합니다.

Stage A is the initial calibration phase of the digital twin system, which fixes the camera model and extrinsic parameters.

## 입력 (Input)

- 센서 구성 (Sensor configuration)
- 마커(기준점) 설계/배치 (Marker/fiducial design and placement)
- Digital Twin Prior Θ0

## 출력 (Output)

- 월드 좌표계 {W} (World coordinate frame {W})
- 캘리브레이션 산출물 (Calibration deliverables):
  - 카메라 내부 파라미터 (Camera intrinsics)
  - 카메라 외부 파라미터 (Camera extrinsics)
  - 태그/마커 위치 정보 (Tag/marker position information)

## 구성 방법 (Configuration)

### 1. 카메라 내부 파라미터 (Camera Intrinsics)

**방법 1: 자동 로딩 (권장 - ZED 카메라용)**

ZED 카메라를 사용하는 경우, camera_info 토픽에서 캘리브레이션을 자동으로 가져올 수 있습니다.

For ZED camera, you can automatically load calibration from the camera_info topic.

```yaml
camera:
  auto_load_from_camera_info: true  # ZED 카메라의 내장 캘리브레이션 사용
```

이렇게 설정하면 별도의 내부 파라미터 입력 없이 ZED 카메라에서 자동으로 캘리브레이션을 받아옵니다.
시스템은 첫 번째 camera_info 메시지를 받을 때까지 기본값(fx=700, fy=700, cx=640, cy=360)을 사용합니다.

With this setting, calibration is automatically obtained from the ZED camera without manual parameter input.
The system uses default values (fx=700, fy=700, cx=640, cy=360) until the first camera_info message is received.

**중요:** ZED 카메라를 먼저 실행한 후 LIVM 노드를 시작하거나, camera_info 토픽이 발행될 때까지 기다려야 합니다.
**Important:** Launch the ZED camera before starting the LIVM node, or wait until the camera_info topic is being published.

**방법 2: 수동 설정**

카메라 캘리브레이션으로부터 얻은 내부 파라미터를 수동으로 설정합니다.

Manually set the intrinsic parameters obtained from camera calibration.

```yaml
camera:
  auto_load_from_camera_info: false  # 수동 설정 모드
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

**참고 (Note):** 
- ZED 카메라를 사용하는 경우 `auto_load_from_camera_info: true` 사용을 권장합니다.
- 자동 로딩 모드에서는 카메라 실행 후 첫 번째 camera_info 메시지를 받으면 즉시 캘리브레이션이 로드됩니다.
- For ZED camera, using `auto_load_from_camera_info: true` is recommended.
- In auto-load mode, calibration is loaded immediately upon receiving the first camera_info message after camera launch.

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
  # 참고: 현재 버전에서는 로딩되지 않음 - Tag-Plane 시스템 구현 시 추가 예정
  # Note: Not loaded in current version - will be added when Tag-Plane system is implemented
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

**중요 (Important):** 
- 태그 위치는 임시값이며, 향후 Tag-Plane 기반 시스템으로 교체될 예정입니다.
- 현재 버전에서는 `tag_size`와 `tag_family`만 로딩됩니다. `tag_positions`는 Tag-Plane 시스템 구현 시 추가될 예정입니다.

Tag positions are temporary values and will be replaced with a Tag-Plane based system in the future.
Current version only loads `tag_size` and `tag_family`. `tag_positions` will be added when Tag-Plane system is implemented.

## 사용 방법 (Usage)

### 런타임 로그 확인 (Check Runtime Logs)

시스템 시작 시 다음과 같은 로그가 출력됩니다:

The following logs are printed at system startup:

**자동 로딩 모드 (Auto-load mode):**
```
[INFO] === Stage A: Camera Calibration Parameters ===
[INFO] Camera Intrinsics: Will be auto-loaded from camera_info topic
[INFO] Subscribed to camera_info topic: /zed/zed_node/rgb/camera_info
...
[Stage A] Camera intrinsics auto-loaded from camera_info:
  fx: 527.82, fy: 527.82
  cx: 641.06, cy: 366.21
  Distortion [k1, k2, p1, p2, k3]: [-0.042123, 0.010456, 0.000123, -0.000456, 0.000789]
[INFO] Camera Extrinsics (Camera to LiDAR):
[INFO]   Translation: [0.000, 0.000, 0.000]
[INFO]   Rotation matrix loaded (3x3)
[INFO] Tag Parameters (temporary, to be replaced with Tag-Plane):
[INFO]   Tag size: 0.100 m
[INFO]   Tag family: tag36h11
[INFO]   Note: Tag positions loading not implemented yet
[INFO]         Will be added when Tag-Plane system is implemented
[INFO] ==============================================
```

**수동 설정 모드 (Manual configuration mode):**
```
[INFO] === Stage A: Camera Calibration Parameters ===
[INFO] Camera Intrinsics (loaded from config):
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
[INFO]   Note: Tag positions loading not implemented yet
[INFO]         Will be added when Tag-Plane system is implemented
[INFO] ==============================================
```

### 파라미터 업데이트 (Update Parameters)

**ZED 카메라 사용 시 (Using ZED Camera):**

1. `config/velodyne.yaml` 파일을 엽니다
2. `camera.auto_load_from_camera_info: true`로 설정되어 있는지 확인합니다
3. ZED 카메라를 실행합니다: `ros2 launch zed_wrapper zed_camera.launch.py camera_model:=zed2i`
4. LIVM 노드를 실행합니다: 카메라 캘리브레이션이 자동으로 로드됩니다

**수동 설정 시 (Manual Configuration):**

1. 해당 센서의 config 파일을 엽니다 (예: `config/avia.yaml`)
2. `camera.auto_load_from_camera_info: false`로 설정합니다
3. `camera.intrinsics` 섹션의 값을 업데이트합니다
4. 노드를 재시작하여 새 파라미터를 로드합니다

## 다음 단계 (Next Steps)

1. **Tag-Plane 시스템 구현**: 임시 태그 위치를 Tag-Plane 기반 시스템으로 교체
2. **카메라 모델 활용**: 고정된 카메라 파라미터를 활용한 이미지 처리 파이프라인 구현
3. **검증**: 캘리브레이션 정확도 검증 및 테스트

## 참고사항 (Notes)

- **ZED 카메라 사용자**: `auto_load_from_camera_info: true` 설정으로 별도의 캘리브레이션 파일 없이 자동으로 사용할 수 있습니다.
- **Velodyne LiDAR 기본 설정**: `config/velodyne.yaml`에는 ZED 카메라 자동 로딩이 기본으로 활성화되어 있습니다.
- 카메라 내부 파라미터는 Stage A에서 고정되며, 실행 중에는 변경되지 않습니다.
- 태그 파라미터는 임시값으로, 향후 Tag-Plane 시스템으로 교체될 예정입니다.
- 자동 로딩은 첫 번째 camera_info 메시지 수신 시 한 번만 실행됩니다.
