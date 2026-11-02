# C4IMap 고도 레이어

## 설치 위치

- SDK 프로젝트: `D:/work1/libsrc-sdk-work-20260917/libsrc/TerrainElevation/TerrainElevation.vcxproj`
- SDK 솔루션: `D:/work1/libsrc-sdk-work-20260917/libsrc/TerrainElevation.sln`
- DLL: `D:/work1/libsrc-sdk-work-20260917/libsrc/bin/x64/Release/TerrainElevation.dll`
- 실행 파일 옆에도 같은 DLL이 배포됩니다. 기존 C4IMap의 GDAL 런타임을 사용합니다.
- 현재 실제 데이터: `D:/map/strm/SRTM2/DTED`, DTED2 69개.

## 사용 방법

지도레이어관리의 **기본지도 → 고도 데이터 (DTED/SRTM)**를 체크합니다.
고도 영상은 모든 다른 지도보다 아래에 그려집니다. Natural Earth·래스터·CIB 등 불투명 영상이 켜져 있으면 고도 영상을 가립니다. 고도 음영 자체를 보려면 상위 배경영상 체크를 해제하세요.

마우스를 지도 위에 이동하면 상태줄 Alt에 높이가 표시됩니다. 영상 체크를 해제해도 고도 조회는 유지됩니다.
- `Alt: 123.4 m`: 소스 데이터의 고도(미터).
- `Alt: ...`: 비동기 조회 대기 중.
- `Alt: N/A`: 데이터 범위 밖, NoData 또는 DLL/데이터가 없는 경우.

DTED/SRTM의 수직 기준을 그대로 사용하며 타원체고로 변환하지 않습니다. 0m와 음수 고도는 유효한 값입니다.

## XML

실행 파일과 같은 폴더의 `base-map.xml`:

```xml
<Elevation>
  <Path>D:/map/strm</Path>
  <visual>true</visual>
</Elevation>
```

모든 하위 폴더를 검색합니다. 체크 변경은 visual에 저장됩니다. 파일을 추가하거나 경로를 변경한 경우 다시 실행하여 색인을 갱신합니다.

## 빌드

Visual Studio 2019(v142), Release x64, C++17, /MD.
SDK의 `GdalSdkDir` 기본값은 `D:/work/상황도/gdal_sdk`입니다. 다른 앱에서 DLL을 사용할 때도 이 SDK와 일치하는 GDAL/PROJ 런타임과 PROJ 데이터베이스를 사용해야 합니다. 다른 PostGIS의 proj.db를 가리키지 않도록 설정하세요.
C4IMap 통합/로컬 프로젝트를 빌드하면 고도 DLL도 빌드하여 실행 폴더에 복사합니다. 네트워크 전용 구성은 로컬 고도 기능을 사용하지 않습니다.

## DLL API

`TerrainElevation.h`에 C ABI를 정의했습니다. 문자열은 UTF-8, 좌표는 WGS84 경도/위도, 고도는 미터입니다.
Create → RequestView/RequestSample → GetStatus/CopyFrame/GetSample → Destroy 순서입니다. 요청은 비동기이며 최신 ticket으로 결과를 확인합니다. 이전 화면 결과는 새 화면에 사용하지 않습니다. DLL 경계를 넘어 메모리를 해제하거나 C++ STL 객체를 전달하지 않습니다.

DTED2, DTED1/0, SRTM HGT 및 지리참조·좌표계가 있는 단일 밴드 DEM 래스터를 처리합니다. 내부 화면 표면은 최대 1024픽셀 변 길이로 제한하여 표시하고, 마우스 고도는 원본 DEM 셀에서 별도로 조회합니다.

## 검증

생성 DTED2와 실제 데이터로 재귀 검색, NoData/0m/음수, 비동기 렌더링, 오래된 요청 무효화, 경도 반복, 요청 중 종료를 검증했습니다.
실제 69개 파일을 인식했으며 경도 127.031 / 위도 37.0861의 원본 고도 13m와 DLL 결과가 일치합니다.

VS2019 Release x64의 통합/로컬/네트워크 빌드를 완료했습니다. 별도 실행 폴더에서 실제 고도 음영 표시, 트리 체크 해제/재설정, XML 저장 및 Alt 표시를 확인했습니다. 고도 레이어를 숨겨도 Alt 조회가 유지됩니다.
