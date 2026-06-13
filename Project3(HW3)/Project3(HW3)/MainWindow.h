#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QMessageBox>
#include <QMap>
#include <QList>
#include <QString>
#include <QFrame>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QFont>
#include <QSet>
#include <QLocale>

// ══════════════════════════════════════════
// 데이터 구조 정의
// ══════════════════════════════════════════

// 열차 정보를 저장하는 구조체 (그래프의 엣지)
struct Train {
    QString trainNo;  // 열차 번호 (예: 무궁화-101, KTX-201)
    int     fromId;   // 출발역 ID (그래프 엣지의 시작 노드)
    int     toId;     // 도착역 ID (그래프 엣지의 끝 노드)
    int     minutes;  // 소요시간 (엣지의 가중치1)
    int     price;    // 가격     (엣지의 가중치2)
};

// 역 정보를 저장하는 구조체 (그래프의 노드)
struct Station {
    int     id;       // 역 고유 번호
    QString name;     // 역 이름
    int     x, y;    // 화면에 표시될 좌표
};

// ══════════════════════════════════════════
// MapCanvas : 노선도를 직접 그리는 위젯
// ══════════════════════════════════════════
class MapCanvas : public QWidget {
    Q_OBJECT
public:
    // 생성자 : 위젯 최소 크기와 배경색 설정
    explicit MapCanvas(QWidget* parent = nullptr) : QWidget(parent) {
        setMinimumSize(720, 400);
        setStyleSheet("background-color: #0f1626;");
    }

    // 역(노드)과 열차(엣지) 데이터를 받아서 저장 후 화면 갱신
    void setData(const QList<Station>& s, const QList<Train>& t) {
        stations = s; trains = t; update();
    }

    // 선택된 역 ID를 저장하고 화면 갱신 (클릭 시 하이라이트 효과)
    void setSelectedStation(int id) { selectedId = id; update(); }

signals:
    // 역을 클릭했을 때 MainWindow로 전달하는 시그널
    void stationClicked(int stationId);

protected:
    // 화면을 그리는 함수 (Qt가 자동으로 호출)
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing); // 부드러운 곡선 처리

        // ── 0. 배경 및 격자선 ──
        p.fillRect(rect(), QColor("#0f1626")); // 배경색 채우기

        // 격자선 그리기 (60px 간격)
        p.setPen(QPen(QColor("#1e2d45"), 1));
        for (int x = 0; x < width(); x += 60)
            p.drawLine(x, 0, x, height());
        for (int y = 0; y < height(); y += 60)
            p.drawLine(0, y, width(), y);

        // ── 1. 전라도 지형 폴리곤 그리기 ──

        // 전라북도 영역 (단순화된 다각형)
        QPolygon jeonbuk;
        jeonbuk << QPoint(50, 80)    // 서해안 북
            << QPoint(130, 60)   // 익산 북쪽
            << QPoint(220, 65)   // 전주 북쪽
            << QPoint(310, 80)   // 동쪽 경계(충북)
            << QPoint(370, 120)  // 동쪽 경계(경북)
            << QPoint(380, 180)  // 남원 동쪽(경남 경계)
            << QPoint(340, 220)  // 남원 남쪽
            << QPoint(280, 240)  // 전남 북쪽
            << QPoint(200, 230)  // 내륙
            << QPoint(120, 220)  // 서쪽
            << QPoint(50, 180)   // 서해안 남
            << QPoint(40, 130)   // 서해안
            << QPoint(50, 80);   // 시작점으로 닫기
        p.setPen(QPen(QColor("#2a3a5a"), 2));
        p.setBrush(QColor("#1a2640")); // 전북 배경색
        p.drawPolygon(jeonbuk);

        // 전라남도 영역 (단순화된 다각형)
        QPolygon jeonnam;
        jeonnam << QPoint(280, 240)  // 전남 북쪽(전북 경계)
            << QPoint(340, 220)  // 남원 남쪽
            << QPoint(390, 240)  // 구례 동쪽(경남 경계)
            << QPoint(430, 270)  // 순천 동쪽
            << QPoint(460, 310)  // 여수 북쪽
            << QPoint(440, 340)  // 여수
            << QPoint(380, 350)  // 고흥 방향
            << QPoint(290, 345)  // 장흥 방향
            << QPoint(200, 340)  // 해남 방향
            << QPoint(120, 330)  // 목포 방향
            << QPoint(80, 290)   // 서해안 남
            << QPoint(100, 250)  // 서해안 북
            << QPoint(200, 230)  // 전북 경계
            << QPoint(280, 240); // 시작점으로 닫기
        p.setPen(QPen(QColor("#2a3a5a"), 2));
        p.setBrush(QColor("#1e2d3e")); // 전남 배경색
        p.drawPolygon(jeonnam);

        // 지역명 텍스트 표시
        p.setPen(QColor("#2e4060"));
        p.setFont(QFont("Arial", 9));
        p.drawText(100, 150, "전라북도");
        p.drawText(180, 300, "전라남도");

        // ── 2. 엣지(열차 노선) 그리기 ──
        QSet<QString> drawn; // 이미 그린 노선 중복 방지용
        for (const Train& t : trains) {
            // 양방향 노선의 중복 그리기 방지 (작은ID-큰ID 형태로 키 생성)
            QString key = QString("%1-%2")
                .arg(qMin(t.fromId, t.toId))
                .arg(qMax(t.fromId, t.toId));
            if (drawn.contains(key)) continue;
            drawn.insert(key);

            // 출발역과 도착역의 좌표 찾기
            Station from{}, to{};
            for (const Station& s : stations) {
                if (s.id == t.fromId) from = s;
                if (s.id == t.toId)   to = s;
            }

            // 선택된 역과 연결된 노선은 밝게 표시
            bool hi = (selectedId == t.fromId || selectedId == t.toId);
            bool isKTX = t.trainNo.startsWith("KTX");

            QPen linePen;
            if (isKTX) {
                // KTX 직통: 주황색 점선
                linePen = QPen(hi ? QColor("#fb923c") : QColor("#78350f"), hi ? 2 : 1);
                linePen.setStyle(Qt::DashLine);
            }
            else {
                // 무궁화 인접: 파란색 실선
                linePen = QPen(hi ? QColor("#4fc3f7") : QColor("#3a4a6b"), hi ? 3 : 2);
            }
            p.setPen(linePen);
            p.drawLine(from.x, from.y, to.x, to.y); // 역과 역 사이 선 그리기

            // 무궁화 구간만 소요시간 표시 (KTX는 생략하여 깔끔하게)
            if (!isKTX) {
                int mx = (from.x + to.x) / 2;
                int my = (from.y + to.y) / 2 - 10;
                p.setPen(QColor("#7a8caa"));
                p.setFont(QFont("Arial", 8));
                p.drawText(mx - 12, my, QString("%1분").arg(t.minutes));
            }
        }

        // ── 범례 표시 ──
        p.setPen(QPen(QColor("#4fc3f7"), 2));
        p.drawLine(10, height() - 30, 40, height() - 30);
        p.setPen(QColor("#94a3b8")); p.setFont(QFont("Arial", 9));
        p.drawText(44, height() - 25, "무궁화 (인접)");
        QPen legendKtx(QColor("#fb923c"), 1); legendKtx.setStyle(Qt::DashLine);
        p.setPen(legendKtx);
        p.drawLine(160, height() - 30, 190, height() - 30);
        p.setPen(QColor("#94a3b8"));
        p.drawText(194, height() - 25, "KTX (직통)");

        // ── 3. 노드(역) 그리기 ──
        for (const Station& s : stations) {
            bool sel = (s.id == selectedId); // 현재 선택된 역인지 확인

            // 선택된 역은 외곽에 반투명 원 추가 (하이라이트 효과)
            if (sel) {
                p.setPen(Qt::NoPen);
                p.setBrush(QColor(79, 195, 247, 55));
                p.drawEllipse(s.x - R - 8, s.y - R - 8, (R + 8) * 2, (R + 8) * 2);
            }

            // 역 원 그리기 (선택 여부에 따라 색상 변경)
            p.setPen(QPen(sel ? QColor("#4fc3f7") : QColor("#4a6fa5"), sel ? 3 : 2));
            p.setBrush(sel ? QColor("#0288d1") : QColor("#263554"));
            p.drawEllipse(s.x - R, s.y - R, R * 2, R * 2);

            // 역 이름 텍스트 (이름 길이에 따라 폰트 크기 조정)
            p.setPen(Qt::white);
            QFont f("Arial", s.name.length() > 3 ? 7 : 8, QFont::Bold);
            p.setFont(f);
            p.drawText(QRect(s.x - R, s.y - R, R * 2, R * 2), Qt::AlignCenter, s.name);
        }
    }

    // 마우스 클릭 이벤트 처리
    void mousePressEvent(QMouseEvent* event) override {
        QPoint pos = event->pos();
        for (const Station& s : stations) {
            int dx = pos.x() - s.x, dy = pos.y() - s.y;
            // 클릭 위치가 역 원 안에 있으면 시그널 발생
            if (dx * dx + dy * dy <= R * R * 2) { emit stationClicked(s.id); return; }
        }
    }

private:
    QList<Station> stations;  // 역 목록 (그래프 노드)
    QList<Train>   trains;    // 열차 목록 (그래프 엣지)
    int selectedId = -1;      // 현재 선택된 역 ID (-1이면 선택 없음)
    const int R = 22;         // 역 원의 반지름
};

// ══════════════════════════════════════════
// MainWindow : 메인 창 (전체 UI 관리)
// ══════════════════════════════════════════
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    // 생성자 : 창 제목, 크기, 스타일 설정 후 데이터와 UI 초기화
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("전라선 기차 노선도");
        resize(1100, 520);
        setStyleSheet("QMainWindow { background-color: #111827; }");
        buildData(); // 그래프 데이터 초기화
        buildUI();   // UI 구성
    }

private slots:
    // 역 클릭 시 호출되는 슬롯 : 선택 역 하이라이트 + 정보 패널 업데이트
    void onStationClicked(int id) {
        mapCanvas->setSelectedStation(id);
        updateInfoPanel(id);
    }

private:
    // ── 그래프 데이터 ──
    QList<Station>        stations; // 노드 목록
    QList<Train>          trains;   // 엣지 목록
    QMap<int, QList<int>> adjList;  // 인접 리스트 (key: 출발역ID, value: 열차 인덱스 목록)

    // ── UI 컴포넌트 ──
    MapCanvas* mapCanvas;        // 노선도 캔버스
    QLabel* stationNameLabel; // 선택된 역 이름 표시
    QWidget* trainListWidget;  // 열차 목록 표시 영역
    QLabel* hintLabel;        // 하단 안내 문구

    // 그래프 데이터 초기화 함수
    void buildData() {
        // 역 정보 (노드) : id, 이름, x좌표, y좌표
        stations = {
            {0, "익산",       130, 145},
            {1, "전주",       195, 165},
            {2, "남원",       310, 210},
            {3, "곡성",       355, 255},
            {4, "구례구",     385, 275},
            {5, "순천",       415, 295},
            {6, "여천",       435, 318},
            {7, "여수엑스포", 448, 335},
        };

        // 열차 정보 (엣지) : 열차번호, 출발역, 도착역, 소요시간, 가격
        trains = {
            // 무궁화 인접 구간 (양방향)
            {"무궁화-101", 0,1, 20, 3500}, {"무궁화-102", 1,0, 20, 3500},
            {"무궁화-103", 1,2, 40, 6200}, {"무궁화-104", 2,1, 40, 6200},
            {"무궁화-105", 2,3, 20, 3100}, {"무궁화-106", 3,2, 20, 3100},
            {"무궁화-107", 3,4, 15, 2400}, {"무궁화-108", 4,3, 15, 2400},
            {"무궁화-109", 4,5, 20, 3200}, {"무궁화-110", 5,4, 20, 3200},
            {"무궁화-111", 5,6, 15, 2100}, {"무궁화-112", 6,5, 15, 2100},
            {"무궁화-113", 6,7, 10, 1800}, {"무궁화-114", 7,6, 10, 1800},

            // KTX 직통 - 익산 출발 (양방향)
            {"KTX-201", 0,2,  60,  9700}, {"KTX-202", 2,0,  60,  9700},
            {"KTX-203", 0,5, 100, 22000}, {"KTX-204", 5,0, 100, 22000},
            {"KTX-205", 0,7, 120, 27600}, {"KTX-206", 7,0, 120, 27600},

            // KTX 직통 - 전주 출발 (양방향)
            {"KTX-301", 1,5,  80, 18500}, {"KTX-302", 5,1,  80, 18500},
            {"KTX-303", 1,7, 100, 24100}, {"KTX-304", 7,1, 100, 24100},

            // KTX 직통 - 남원 출발 (양방향)
            {"KTX-401", 2,5,  55, 12300}, {"KTX-402", 5,2,  55, 12300},
            {"KTX-403", 2,7,  75, 17900}, {"KTX-404", 7,2,  75, 17900},

            // KTX 직통 - 순천 출발 (양방향)
            {"KTX-501", 5,7,  20,  5600}, {"KTX-502", 7,5,  20,  5600},
        };

        // 인접 리스트 구성 : 각 역에서 출발하는 열차 인덱스를 저장
        for (int i = 0; i < trains.size(); i++)
            adjList[trains[i].fromId].append(i);
    }

    // UI 구성 함수
    void buildUI() {
        // 중앙 위젯 설정
        auto* central = new QWidget(this);
        setCentralWidget(central);
        central->setStyleSheet("background-color: #111827;");

        // 좌우 분할 레이아웃
        auto* main = new QHBoxLayout(central);
        main->setContentsMargins(12, 12, 12, 12);
        main->setSpacing(12);

        // ── 왼쪽 패널 : 노선도 ──
        auto* leftBox = new QWidget;
        leftBox->setStyleSheet("background-color:#1a1f2e; border:1px solid #2a3a5a; border-radius:10px;");
        auto* leftL = new QVBoxLayout(leftBox);
        leftL->setContentsMargins(10, 10, 10, 10);

        // 제목 라벨
        auto* title = new QLabel("🚄  전라선 노선도");
        title->setStyleSheet("color:#4fc3f7; font-size:15px; font-weight:bold; border:none; padding:4px 0;");
        leftL->addWidget(title);

        // 노선도 캔버스 생성 및 시그널-슬롯 연결
        mapCanvas = new MapCanvas(leftBox);
        mapCanvas->setData(stations, trains);
        connect(mapCanvas, &MapCanvas::stationClicked, this, &MainWindow::onStationClicked);
        leftL->addWidget(mapCanvas);

        // 하단 안내 문구
        hintLabel = new QLabel("역을 클릭하면 열차 정보가 표시됩니다");
        hintLabel->setStyleSheet("color:#4a6fa5; font-size:11px; border:none; padding:4px 0;");
        hintLabel->setAlignment(Qt::AlignCenter);
        leftL->addWidget(hintLabel);
        main->addWidget(leftBox, 7); // 전체 너비의 70%

        // ── 오른쪽 패널 : 열차 정보 ──
        auto* rightBox = new QWidget;
        rightBox->setMinimumWidth(270);
        rightBox->setMaximumWidth(310);
        rightBox->setStyleSheet("background-color:#1a1f2e; border:1px solid #2a3a5a; border-radius:10px;");
        auto* rightL = new QVBoxLayout(rightBox);
        rightL->setContentsMargins(14, 14, 14, 14);
        rightL->setSpacing(8);

        // 선택된 역 이름 표시 라벨
        stationNameLabel = new QLabel("역을 선택하세요");
        stationNameLabel->setStyleSheet(
            "color:#e2e8f0; font-size:18px; font-weight:bold;"
            "border-bottom:2px solid #2a3a5a; padding-bottom:8px; border-radius:0;");
        stationNameLabel->setWordWrap(true);
        rightL->addWidget(stationNameLabel);

        // 열차 목록을 스크롤 가능하게 표시
        auto* scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setStyleSheet("QScrollArea{border:none;background:transparent;}"
            "QScrollBar:vertical{background:#1a1f2e;width:6px;}"
            "QScrollBar::handle:vertical{background:#3a4a6b;border-radius:3px;}");

        trainListWidget = new QWidget;
        trainListWidget->setStyleSheet("background:transparent;");
        new QVBoxLayout(trainListWidget);
        scroll->setWidget(trainListWidget);
        rightL->addWidget(scroll, 1);
        main->addWidget(rightBox, 3); // 전체 너비의 30%
    }

    // 오른쪽 정보 패널 업데이트 함수
    void updateInfoPanel(int stationId) {
        // 선택된 역 이름 찾기
        QString stationName;
        for (const Station& s : stations)
            if (s.id == stationId) { stationName = s.name; break; }

        stationNameLabel->setText(QString("🚉  %1역").arg(stationName));

        // 기존 열차 목록 제거
        QLayout* old = trainListWidget->layout();
        QLayoutItem* item;
        while ((item = old->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        auto* layout = qobject_cast<QVBoxLayout*>(old);

        // 출발 열차 섹션 라벨
        auto* depLabel = new QLabel("▶  출발 열차");
        depLabel->setStyleSheet("color:#4fc3f7; font-size:12px; font-weight:bold; margin-top:8px;");
        layout->addWidget(depLabel);

        // 인접 리스트에서 해당 역의 출발 열차 목록 조회
        bool hasDep = false;
        for (int idx : adjList[stationId]) {
            const Train& t = trains[idx];

            // 도착역 이름 찾기
            QString toName;
            for (const Station& s : stations)
                if (s.id == t.toId) { toName = s.name; break; }

            // 열차 카드 UI 생성
            auto* card = new QFrame;
            card->setStyleSheet("QFrame{background-color:#212d42; border:1px solid #2e3f5c; border-radius:8px; padding:2px;}");
            auto* cL = new QVBoxLayout(card);
            cL->setContentsMargins(10, 8, 10, 8);
            cL->setSpacing(4);

            // 열차번호 및 목적지 표시
            auto* row1 = new QLabel(
                QString("<b style='color:#e2e8f0;'>%1</b><span style='color:#7a8caa;'>  →  %2역</span>")
                .arg(t.trainNo).arg(toName));
            row1->setStyleSheet("font-size:12px;");

            // 소요시간 및 가격 표시
            auto* row2 = new QLabel(
                QString("<span style='color:#94a3b8;'>⏱ %1분 &nbsp;&nbsp;</span><span style='color:#4ade80;'>₩%2</span>")
                .arg(t.minutes)
                .arg(QLocale(QLocale::Korean).toString(t.price)));
            row2->setStyleSheet("font-size:11px;");

            // 예매하기 버튼
            auto* btn = new QPushButton("예매하기");
            btn->setStyleSheet(
                "QPushButton{background-color:#0369a1;color:white;border:none;border-radius:5px;padding:5px 10px;font-size:11px;font-weight:bold;}"
                "QPushButton:hover{background-color:#0284c7;}"
                "QPushButton:pressed{background-color:#0ea5e9;}");

            // 람다로 예매 완료 팝업 연결 (변수 캡처)
            QString tn = t.trainNo, fn = stationName, ton = toName;
            int mi = t.minutes, pr = t.price;
            connect(btn, &QPushButton::clicked, this, [=]() {
                QMessageBox msg(this);
                msg.setWindowTitle("✅  예매 완료");
                msg.setText(QString(
                    "<h3 style='color:#22c55e;'>예매가 완료되었습니다!</h3>"
                    "<p style='color:#e2e8f0;'>"
                    "열차편: <b>%1</b><br>"
                    "구간: <b>%2 → %3</b><br>"
                    "소요시간: <b>%4분</b><br>"
                    "가격: <b>₩%5</b></p>")
                    .arg(tn).arg(fn).arg(ton).arg(mi)
                    .arg(QLocale(QLocale::Korean).toString(pr)));
                msg.setStyleSheet(
                    "QMessageBox{background-color:#1a1f2e;}"
                    "QLabel{color:#e2e8f0;}"
                    "QPushButton{background-color:#0369a1;color:white;border:none;border-radius:5px;padding:6px 20px;}");
                msg.exec();
                });

            cL->addWidget(row1);
            cL->addWidget(row2);
            cL->addWidget(btn);
            layout->addWidget(card);
            hasDep = true;
        }

        // 출발 열차가 없는 경우 메시지 표시
        if (!hasDep) {
            auto* none = new QLabel("출발 열차 없음");
            none->setStyleSheet("color:#4a6fa5; font-size:11px; padding:4px;");
            layout->addWidget(none);
        }
        layout->addStretch();

        // 하단 안내 문구 업데이트
        hintLabel->setText(QString("선택된 역: %1역").arg(stationName));
    }
};