# C4IMap Terrain 파일 / Cesium terrain 서버

## 실행과 설정

통합 실행 파일: `D:/work/상황도/Release/Bin/C4IMap.exe`
로컬 전용: `D:/work/상황도/c4imap/dist/Local/C4IMap.exe`
서버 전용: `D:/work/상황도/c4imap/dist/Network/C4IMap.exe`

**실행 파일 옆의 `base-map.xml`**에서 `<BaseMap>` 안에 아래 항목을 설정합니다. 기존의 SmallScale, World, Elevation 등 다른 항목은 유지하세요.

```xml
<TerrainLocal>
  <LayerJson>D:/map/terrain/layer.json</LayerJson>
  <visual>false</visual>
</TerrainLocal>
<TerrainServer>
  <LayerJson>http://서버주소:8000/tilesets/데이터셋/layer.json</LayerJson>
  <visual>false</visual>
</TerrainServer>
```

위 경로와 서버 주소는 예시입니다. 현재 실제 terrain 파일/운영 서버가 지정되지 않아 로컬 경로는 예시값, 서버 주소는 빈 값, 둘 다 false로 배포했습니다. 유효한 경로 또는 서버 주소를 넣고 다시 실행한 뒤 트리에서 체크하세요. URL의 `&`는 XML에서 `&amp;`로 씁니다.

- 지도레이어관리 → 기본지도 → **Terrain 파일 (로컬)**
- 지도레이어관리 → 기본지도 → **Terrain 서버 (Cesium)**
- 체크 변경은 XML의 visual에 저장됩니다. 경로 변경 후에는 다시 실행합니다.
- 로컬은 layer.json 파일 또는 그 파일이 들어 있는 디렉터리를 지정합니다. `{z}/{x}/{y}.terrain` 구조와 layer.json을 함께 보존하세요. 확장자만으로 위치를 결정할 수 없어 임의 파일 목록을 읽지는 않습니다.
- 서버는 데이터셋의 layer.json URL 또는 데이터셋 디렉터리 URL을 지정합니다. cesium-terrain-server의 기본 경로는 `/tilesets/데이터셋/layer.json`입니다.
- 원래의 DTED/SRTM **고도 데이터** 항목은 별도로 유지됩니다.

## 화면과 고도

C4IMap의 기존 평면 지도에 고도 색상과 음영을 표시합니다. Cesium 3D 지구/카메라 뷰어를 추가한 것은 아닙니다.
배경색 → DTED/SRTM → Terrain 로컬 → Terrain 서버 → 기존 지도 순서로 표시합니다. 상위 Natural Earth, 래스터, CIB 등 불투명 영상을 켜면 terrain이 가려질 수 있습니다.

표시 체크와 고도 조회는 독립적입니다. 고도 영상은 기본적으로 숨겨지고, 체크할 때만 표시됩니다. 체크를 해제해도 Alt 조회는 유지됩니다. base-map.xml의 `<AltitudeSource>auto</AltitudeSource>`로 조회 대상을 정합니다. auto는 서버 주소가 있으면 서버, 유효한 로컬 layer.json이 있으면 로컬 terrain, 나머지는 DTED/SRTM 순서입니다. `dted`, `terrain-local`, `terrain-server`로 고정할 수도 있습니다. 배포에서 비활성화된 backend는 auto 선택에서 제외됩니다. `...`는 대기, `N/A`는 데이터 없음 또는 설정/접속 오류입니다.
고도는 현재 표시 LOD의 타일에서 보간한 값입니다. 원본 DEM의 정확한 셀 조회와는 차이가 있을 수 있습니다. 타일에 저장된 수직 기준을 유지하며 타원체고/해발고도 변환은 수행하지 않습니다.

## 형식과 성능

- `layer.json`의 `quantized-mesh-1.0`, `heightmap-1.0` 지원
- gzip 압축/비압축, 16/32비트 mesh 인덱스 지원
- EPSG:4326 / EPSG:3857, tms / slippyMap 타일 방향 처리
- version, tiles, minzoom/maxzoom, available 처리
- 정적 타일셋 대상. parentUrl로 연결된 여러 tileset의 합성 및 Cesium ion 인증은 지원하지 않습니다. 인증 없는 HTTP(S) 또는 접근 가능한 내부 서버 URL을 사용하세요.
- missing/실패한 타일은 가능한 상위 타일로 대체합니다. 실패 결과도 현재 실행의 메모리에 저장됩니다. 서버 복구/파일 수정 후에는 다시 실행하세요.
- 백그라운드 읽기/렌더링, 128개 타일 메모리 캐시, 축척에 따른 LOD, 오래된 화면 요청 무효화
- 타일당 129×129 보간 표면, 화면 최대 변 1024픽셀. 지도 확대/축소 시 해당 LOD로 다시 생성합니다.
- HTTP 연결 2초/요청 3초 제한. 한 번에 처리할 양을 제한하고 남은 타일은 후속 프레임으로 이어서 읽습니다.
- tileset의 최소 zoom에서도 한 화면이 64개보다 많은 타일을 요구하면 확대가 필요하다는 진단을 남깁니다. 저축척용 상위 레벨 타일을 함께 제공하면 넓은 화면을 표시할 수 있습니다.
- 비정상 크기/인덱스/경로를 검사합니다. 확장 데이터 중 normals/watermask는 음영 생성에 사용하지 않습니다.

진단은 실행 파일 옆 `terrain-tiles.log` 및 디버그 출력에 기록됩니다. DLL 로드 실패 시 기존 GDAL 런타임 DLL들이 같은 폴더에 있는지 확인하세요. 파일/메타데이터가 잘못되거나 지원하지 않는 형식이면 빈 지형과 진단이 반환됩니다.

## DLL과 VS2019 빌드

솔루션: `D:/work1/libsrc-sdk-work-20260917/libsrc/TerrainTiles.sln`
소스: `D:/work1/libsrc-sdk-work-20260917/libsrc/TerrainTiles/`
결과: `D:/work1/libsrc-sdk-work-20260917/libsrc/bin/x64/Release/TerrainTiles.dll`

VS2019 v142, C++17, Release x64, /MD. 기존 C4IMap GDAL SDK를 사용합니다.
세 C4IMap 프로젝트가 DLL을 함께 빌드하고 실행 폴더에 복사합니다.

프로젝트 속성 → C/C++ → 전처리기 정의:

|구분|C4MAP_ENABLE_LOCAL|C4MAP_ENABLE_OGC|C4MAP_ENABLE_TERRAIN_LOCAL|C4MAP_ENABLE_TERRAIN_SERVER|
|---|---|---|---|---|
|통합|1|1|1|1|
|로컬 전용|1|0|1|0|
|서버 전용|0|1|0|1|

Terrain 옵션을 직접 지정하지 않으면 LOCAL/OGC 값을 따릅니다. `MapBuildOptions.h`에서 기본값을 정의했습니다. 꺼진 backend는 트리에 나타나지 않으며 해당 파일/서버를 열지 않습니다. TerrainTiles.dll은 공통 DLL이고 실행 프로젝트의 전처리 옵션이 사용 가능한 backend를 제한합니다.

`TerrainTiles.h`는 C ABI입니다. Create(pathUTF8, server=0/1) → RequestView/RequestSample → GetStatus/CopyFrame/GetSample → Destroy 순서로 사용합니다. API 결과 버퍼는 호출자가 소유합니다. 화면 좌표는 WGS84 경위도, 영상은 top-down BGRA입니다. `GetError`로 UTF-8 진단을 가져옵니다. `frameRevision`이 바뀌면 동일 요청 ticket이라도 새 프레임을 복사해야 합니다.

## 검증 범위

테스트 타일을 생성하여 macOS ASan/UBSan, Windows 네이티브 DLL에서 검증했습니다.
quantized-mesh 16/32비트, heightmap, gzip, TMS/slippy, 높이, 화면 커버리지, 날짜변경선 반복, 오래된 요청, 없는 타일 및 잘못된 입력을 확인했습니다.
HTTP fixture 서버에서 압축 응답을 검증했고, 로컬/서버 전용 C4IMap에서 표시/숨김·XML 저장·backend 구분·Alt가 일치하는 것을 확인했습니다.
실제 운영 cesium-terrain-server와 사용자 terrain 타일은 아직 제공되지 않아 해당 서버/데이터 자체의 검증은 남아 있습니다.

형식 참고:
- https://github.com/CesiumGS/quantized-mesh
- https://github.com/geo-data/cesium-terrain-server
- https://github.com/CesiumGS/cesium/blob/main/packages/engine/Source/Core/CesiumTerrainProvider.js
