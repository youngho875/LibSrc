# ModelAssets DLL / Globe3D 3차원 모델

## 설치 위치

- 소스: `D:\work\상황도\LibSrc\ModelAssets`
- Visual Studio 2019 x64 솔루션: `build\ModelAssets.sln`
- DLL: `D:\work\상황도\LibSDK\bin\ModelAssets.dll`
- import library: `D:\work\상황도\LibSDK\lib\ModelAssets.lib`
- 공개 헤더: `D:\work\상황도\LibSDK\include\ModelAssets\ModelAssets.h`
- 실행 시 Globe3D.exe 옆에 ModelAssets.dll이 필요합니다. 기존 GDAL SDK 런타임도 사용합니다.

`ModelAssets.h`의 MA_Load/MA_Destroy와 조회 함수는 C ABI입니다. 반환 데이터는 해당 핸들을 MA_Destroy하기 전까지 유효합니다. 로더는 OpenGL을 사용하지 않으므로 별도 로딩 스레드에서 호출할 수 있습니다. GPU 업로드/해제는 Globe3D의 OpenGL 스레드에서 수행합니다.

## 사용

중앙 `c4imap-paths.xml`의 `ModelSourcesXml`로 XML 위치를 지정합니다. 기본값은 `${ExeDir}/model-sources.xml`입니다. Globe3D 지도에서 마우스 오른쪽 버튼을 누르면 **3차원 객체 표시 / 3차원 객체 XML 불러오기 / 3차원 객체 다시 읽기**가 있습니다. XML 수정 후 다시 읽기를 선택합니다.

```xml
<?xml version="1.0" encoding="utf-8"?>
<ModelSources enabled="true" gpuCacheMB="512" screenError="16"
              minimumPixels="1" maximumInstances="50000">
  <Asset id="building" path="models/building.glb" />
  <Object name="건물" asset="building"
          longitude="127" latitude="37" heading="0"
          scale="1" height="0" radius="100"
          maximumDistance="100000" clampToGround="true" />
</ModelSources>
```

경위도와 heading은 도, 높이·거리·radius는 미터입니다. heading은 북쪽 기준 시계 방향입니다. scale은 배율입니다. 같은 Asset을 여러 Object에서 사용하면 모델과 텍스처를 공유하며, 지원 GPU에서는 인스턴싱으로 그립니다. radius는 로딩 전 컬링에 쓰는 모델 반경이므로 실제 크기를 포함하도록 지정합니다.

`clampToGround="true"`이면 모델 최저점이 해당 위치의 지형 높이 + height에 놓입니다. false이면 모델 원점이 height에 놓입니다. 건물을 지형에 맞춰 변형하는 기능은 아니며, 경사진 지형에서는 모델 기준 위치의 높이를 사용합니다.

기본적으로 Globe3D의 현재 고도 소스를 사용합니다. 모델 전용 고도 소스가 필요하면 다음 요소를 추가합니다.

```xml
<!-- kind=0: 기존 TerrainElevation DLL용 원본 고도 소스 -->
<Elevation kind="0" path="D:/map/strm" />
<!-- kind=1: 로컬 layer.json, kind=2: terrain 서버 layer.json (TerrainTiles DLL) -->
```

고도 로딩은 비동기이며 아직 조회되지 않았거나 데이터가 없으면 일시적으로 0m를 사용합니다. 기존 고도 DLL이 제공하는 높이 기준을 따르며 지오이드 보정은 새 라이브러리에서 자동 수행하지 않습니다.

거리별 간략 모델을 준비했다면 다음과 같이 지정합니다. LOD 모델 자체를 자동 생성하지는 않습니다.

```xml
<Asset id="building">
  <Lod path="models/detail.glb" maximumDistance="2000" />
  <Lod path="models/simple.glb" maximumDistance="100000" />
</Asset>
```

지리 좌표가 포함된 명시적 3D Tiles는 다음과 같습니다.

```xml
<Tileset path="models/city/tileset.json" clampToGround="false" />
```

Tileset은 계층 transform, b3dm RTC_CENTER, WGS84 ECEF 좌표를 사용합니다. 기본값 false는 원래 고도를 유지합니다. true이면 타일 콘텐츠 최저점을 기준으로 타일 전체를 지형에 맞춰 올리거나 내립니다. 개별 건물마다 별도 지형 높이가 필요하면 Object를 따로 배치하거나 타일을 건물 단위로 구성합니다. b3dm을 Asset으로 직접 지정할 때는 XML 경위도로 배치하고, 지리 참조가 필요하면 tileset.json을 사용합니다.

## 대량 표시

두 개의 로더 스레드, 로딩 대기열 제한, 공간 인덱스, 시야·지평선 컬링, 거리별 LOD, 3D Tiles 화면 오차 선택, GPU 캐시/LRU, 동일 모델 GPU 인스턴싱을 사용합니다. REPLACE 타일은 자식이 준비되기 전까지 부모를 유지합니다. GPU 캐시보다 현재 화면의 모델 전체가 크면 재로딩이 늘어날 수 있습니다. `gpuCacheMB`는 64~4096, `maximumInstances`는 1~1000000입니다. 높은 수치는 실제 메모리/표시 성능에 맞춰 사용합니다.

## 지원 범위

- 3DS, glTF 2.0, GLB, b3dm v1과 로컬 명시적 tileset.json
- 정적 삼각형 메시, 기본 색상/텍스처, 투명도, 양면 재질
- Assimp Draco 디코더 포함
- 애니메이션·전체 PBR·KTX2/Basis·implicit tiling·i3dm/pnts/cmpt·타일당 복수 콘텐츠·모델 HTTP 스트리밍은 이번 범위에 포함되지 않습니다.
- 실사용 모델이 제공되지 않아 생성한 시험 파일로 검증했습니다. 외부 모델의 확장/재질 특성에 따라 추가 지원이 필요할 수 있습니다.

## 빌드

Visual Studio 2019 C++/MFC x64, 기존 gdal_sdk가 필요합니다. `powershell -ExecutionPolicy Bypass -File build.ps1`로 빌드합니다. 준비된 의존성은 다음 경로에 있습니다.

- `tools/cmake-3.30.5-windows-x86_64`: 공식 CMake 3.30.5
- `third_party/assimp-5.4.3`: 공식 Assimp v5.4.3

새 환경에서는 https://cmake.org/files/v3.30/ 와 https://github.com/assimp/assimp/releases/tag/v5.4.3 에서 해당 버전을 받아 같은 위치에 풀어 줍니다. Assimp ZIP SHA256은 `795C29716F4AC123B403E53B677E9F32A8605C4A7B2D9904BFAAE3F4053B506D`입니다. CMakeLists.txt가 Assimp/Draco/zlib 정적 라이브러리를 링크하고 LibSDK로 결과를 출력합니다. 배포 시 함께 제공된 licenses/ModelAssets를 포함합니다.

포맷 참고: https://github.com/CesiumGS/3d-tiles/tree/main/specification/TileFormats/Batched3DModel

## 검증 결과 (2026-11-28)

합성 모델 네 형식의 로딩, 외부/내장 텍스처, 한글 경로, 손상된 b3dm 거부 검사를 통과했습니다. Intel Iris Xe / OpenGL 4.6, 800×600 시험 화면에서 동일한 작은 모델 10,000개를 하나의 GPU 자원·1회 draw call로 표시했습니다. 20프레임 평균 11.640ms였습니다. 기본지도나 복잡한 실사용 모델을 포함한 전체 앱 성능을 의미하지 않습니다. ECEF 좌표 변환, 모델 바닥 배치 계산, 표시 끄기, 지리 참조 b3dm 위치, 자식 LOD와 부모 복귀, OpenGL 오류 검사도 통과했습니다.
