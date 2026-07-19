#pragma once

// 접속 방식 컴파일 토글
//  1 = EOS 세션 접속 (원격/인터넷)
//  0 = 직접 IP 접속 (LAN·로컬 테스트용)
// 값 변경 후 반드시 리빌드. 호스트/클라 둘 다 같은 값으로 빌드해야 함.
#define MG_USE_EOS 0

// IP 모드(MG_USE_EOS 0)에서 클라가 접속할 호스트 주소.
//  - 한 PC에서 창 2개: TEXT("127.0.0.1")
//  - LAN 다른 PC: 호스트 PC의 IPv4 (예: TEXT("192.168.0.10"))
#define MG_DIRECT_IP TEXT("127.0.0.1")
