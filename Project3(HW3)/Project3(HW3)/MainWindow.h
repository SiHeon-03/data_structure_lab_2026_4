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

struct Train {
    QString trainNo;
    int     fromId;
    int     toId;
    int     minutes;
    int     price;
};

struct Station {
    int     id;
    QString name;
    int     x, y;
};

class MapCanvas : public QWidget {
    Q_OBJECT
public:
    explicit MapCanvas(QWidget* parent = nullptr) : QWidget(parent) {
        setMinimumSize(720, 400);
        setStyleSheet("background-color: #0f1626;");
    }

    void setData(const QList<Station>& s, const QList<Train>& t) {
        stations = s; trains = t; update();
    }

    void setSelectedStation(int id) { selectedId = id; update(); }

signals:
    void stationClicked(int stationId);

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // 0. 배경
        p.fillRect(rect(), QColor("#0f1626"));

        // 격자선
        p.setPen(QPen(QColor("#1e2d45"), 1));
        for (int x = 0; x < width(); x += 60)
            p.drawLine(x, 0, x, height());
        for (int y = 0; y < height(); y += 60)
            p.drawLine(0, y, width(), y);

        // 1. 전라도 지형 폴리곤
        // 전북(위쪽) + 전남(아래쪽) 영역을 단순화
        QPolygon jeonbuk;
        jeonbuk << QPoint(50, 80)   // 서해안 북
            << QPoint(130, 60)   // 익산 북쪽
            << QPoint(220, 65)   // 전주 북쪽
            << QPoint(310, 80)   // 동쪽 경계(충북)
            << QPoint(370, 120)  // 동쪽 경계(경북)
            << QPoint(380, 180)  // 남원 동쪽(경남 경계)
            << QPoint(340, 220)  // 남원 남쪽
            << QPoint(280, 240)  // 전남 북쪽
            << QPoint(200, 230)  // 내륙
            << QPoint(120, 220)  // 서쪽
            << QPoint(50, 180)  // 서해안 남
            << QPoint(40, 130)  // 서해안
            << QPoint(50, 80);  // 시작점
        p.setPen(QPen(QColor("#2a3a5a"), 2));
        p.setBrush(QColor("#1a2640"));
        p.drawPolygon(jeonbuk);

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
            << QPoint(80, 290)  // 서해안 남
            << QPoint(100, 250)  // 서해안 북
            << QPoint(200, 230)  // 전북 경계
            << QPoint(280, 240); // 시작점
        p.setPen(QPen(QColor("#2a3a5a"), 2));
        p.setBrush(QColor("#1e2d3e"));
        p.drawPolygon(jeonnam);

        // 지명 텍스트
        p.setPen(QColor("#2e4060"));
        p.setFont(QFont("Arial", 9));
        p.drawText(100, 150, "전라북도");
        p.drawText(180, 300, "전라남도");

        // 2. 엣지(선) 그리기
        QSet<QString> drawn;
        for (const Train& t : trains) {
            QString key = QString("%1-%2")
                .arg(qMin(t.fromId, t.toId))
                .arg(qMax(t.fromId, t.toId));
            if (drawn.contains(key)) continue;
            drawn.insert(key);

            Station from{}, to{};
            for (const Station& s : stations) {
                if (s.id == t.fromId) from = s;
                if (s.id == t.toId)   to = s;
            }
            bool hi = (selectedId == t.fromId || selectedId == t.toId);
            bool isKTX = t.trainNo.startsWith("KTX");

            QPen linePen;
            if (isKTX) {
                linePen = QPen(hi ? QColor("#fb923c") : QColor("#78350f"), hi ? 2 : 1);
                linePen.setStyle(Qt::DashLine);
            }
            else {
                linePen = QPen(hi ? QColor("#4fc3f7") : QColor("#3a4a6b"), hi ? 3 : 2);
            }
            p.setPen(linePen);
            p.drawLine(from.x, from.y, to.x, to.y);

            if (!isKTX) {
                int mx = (from.x + to.x) / 2;
                int my = (from.y + to.y) / 2 - 10;
                p.setPen(QColor("#7a8caa"));
                p.setFont(QFont("Arial", 8));
                p.drawText(mx - 12, my, QString("%1분").arg(t.minutes));
            }
        }

        // 범례
        p.setPen(QPen(QColor("#4fc3f7"), 2));
        p.drawLine(10, height() - 30, 40, height() - 30);
        p.setPen(QColor("#94a3b8")); p.setFont(QFont("Arial", 9));
        p.drawText(44, height() - 25, "무궁화 (인접)");
        QPen legendKtx(QColor("#fb923c"), 1); legendKtx.setStyle(Qt::DashLine);
        p.setPen(legendKtx);
        p.drawLine(160, height() - 30, 190, height() - 30);
        p.setPen(QColor("#94a3b8"));
        p.drawText(194, height() - 25, "KTX (직통)");

        // 3. 노드(역) 그리기
        for (const Station& s : stations) {
            bool sel = (s.id == selectedId);
            if (sel) {
                p.setPen(Qt::NoPen);
                p.setBrush(QColor(79, 195, 247, 55));
                p.drawEllipse(s.x - R - 8, s.y - R - 8, (R + 8) * 2, (R + 8) * 2);
            }
            p.setPen(QPen(sel ? QColor("#4fc3f7") : QColor("#4a6fa5"), sel ? 3 : 2));
            p.setBrush(sel ? QColor("#0288d1") : QColor("#263554"));
            p.drawEllipse(s.x - R, s.y - R, R * 2, R * 2);

            p.setPen(Qt::white);
            QFont f("Arial", s.name.length() > 3 ? 7 : 8, QFont::Bold);
            p.setFont(f);
            p.drawText(QRect(s.x - R, s.y - R, R * 2, R * 2), Qt::AlignCenter, s.name);
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        QPoint pos = event->pos();
        for (const Station& s : stations) {
            int dx = pos.x() - s.x, dy = pos.y() - s.y;
            if (dx * dx + dy * dy <= R * R * 2) { emit stationClicked(s.id); return; }
        }
    }

private:
    QList<Station> stations;
    QList<Train>   trains;
    int selectedId = -1;
    const int R = 22;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("전라선 기차 노선도");
        resize(1100, 520);
        setStyleSheet("QMainWindow { background-color: #111827; }");
        buildData();
        buildUI();
    }

private slots:
    void onStationClicked(int id) {
        mapCanvas->setSelectedStation(id);
        updateInfoPanel(id);
    }

private:
    QList<Station>        stations;
    QList<Train>          trains;
    QMap<int, QList<int>>  adjList;

    MapCanvas* mapCanvas;
    QLabel* stationNameLabel;
    QWidget* trainListWidget;
    QLabel* hintLabel;

    void buildData() {
        stations = {
            {0, "익산",       130,  145},
            {1, "전주",       195,  165},
            {2, "남원",       310,  210},
            {3, "곡성",       355,  255},
            {4, "구례구",     385,  275},
            {5, "순천",       415,  295},
            {6, "여천",       435,  318},
            {7, "여수엑스포", 448,  335},
        };
        trains = {
            {"무궁화-101", 0,1, 20, 3500}, {"무궁화-102", 1,0, 20, 3500},
            {"무궁화-103", 1,2, 40, 6200}, {"무궁화-104", 2,1, 40, 6200},
            {"무궁화-105", 2,3, 20, 3100}, {"무궁화-106", 3,2, 20, 3100},
            {"무궁화-107", 3,4, 15, 2400}, {"무궁화-108", 4,3, 15, 2400},
            {"무궁화-109", 4,5, 20, 3200}, {"무궁화-110", 5,4, 20, 3200},
            {"무궁화-111", 5,6, 15, 2100}, {"무궁화-112", 6,5, 15, 2100},
            {"무궁화-113", 6,7, 10, 1800}, {"무궁화-114", 7,6, 10, 1800},

            {"KTX-201", 0,2,  60,  9700}, {"KTX-202", 2,0,  60,  9700},
            {"KTX-203", 0,5, 100, 22000}, {"KTX-204", 5,0, 100, 22000},
            {"KTX-205", 0,7, 120, 27600}, {"KTX-206", 7,0, 120, 27600},

            {"KTX-301", 1,5,  80, 18500}, {"KTX-302", 5,1,  80, 18500},
            {"KTX-303", 1,7, 100, 24100}, {"KTX-304", 7,1, 100, 24100},

            {"KTX-401", 2,5,  55, 12300}, {"KTX-402", 5,2,  55, 12300},
            {"KTX-403", 2,7,  75, 17900}, {"KTX-404", 7,2,  75, 17900},

            {"KTX-501", 5,7,  20,  5600}, {"KTX-502", 7,5,  20,  5600},
        };
        for (int i = 0; i < trains.size(); i++)
            adjList[trains[i].fromId].append(i);
    }

    void buildUI() {
        auto* central = new QWidget(this);
        setCentralWidget(central);
        central->setStyleSheet("background-color: #111827;");

        auto* main = new QHBoxLayout(central);
        main->setContentsMargins(12, 12, 12, 12);
        main->setSpacing(12);

        auto* leftBox = new QWidget;
        leftBox->setStyleSheet("background-color:#1a1f2e; border:1px solid #2a3a5a; border-radius:10px;");
        auto* leftL = new QVBoxLayout(leftBox);
        leftL->setContentsMargins(10, 10, 10, 10);

        auto* title = new QLabel("🚄  전라선 노선도");
        title->setStyleSheet("color:#4fc3f7; font-size:15px; font-weight:bold; border:none; padding:4px 0;");
        leftL->addWidget(title);

        mapCanvas = new MapCanvas(leftBox);
        mapCanvas->setData(stations, trains);
        connect(mapCanvas, &MapCanvas::stationClicked, this, &MainWindow::onStationClicked);
        leftL->addWidget(mapCanvas);

        hintLabel = new QLabel("역을 클릭하면 열차 정보가 표시됩니다");
        hintLabel->setStyleSheet("color:#4a6fa5; font-size:11px; border:none; padding:4px 0;");
        hintLabel->setAlignment(Qt::AlignCenter);
        leftL->addWidget(hintLabel);
        main->addWidget(leftBox, 7);

        auto* rightBox = new QWidget;
        rightBox->setMinimumWidth(270);
        rightBox->setMaximumWidth(310);
        rightBox->setStyleSheet("background-color:#1a1f2e; border:1px solid #2a3a5a; border-radius:10px;");
        auto* rightL = new QVBoxLayout(rightBox);
        rightL->setContentsMargins(14, 14, 14, 14);
        rightL->setSpacing(8);

        stationNameLabel = new QLabel("역을 선택하세요");
        stationNameLabel->setStyleSheet(
            "color:#e2e8f0; font-size:18px; font-weight:bold;"
            "border-bottom:2px solid #2a3a5a; padding-bottom:8px; border-radius:0;");
        stationNameLabel->setWordWrap(true);
        rightL->addWidget(stationNameLabel);

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
        main->addWidget(rightBox, 3);
    }

    void updateInfoPanel(int stationId) {
        QString stationName;
        for (const Station& s : stations)
            if (s.id == stationId) { stationName = s.name; break; }

        stationNameLabel->setText(QString("🚉  %1역").arg(stationName));

        QLayout* old = trainListWidget->layout();
        QLayoutItem* item;
        while ((item = old->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        auto* layout = qobject_cast<QVBoxLayout*>(old);

        auto* depLabel = new QLabel("▶  출발 열차");
        depLabel->setStyleSheet("color:#4fc3f7; font-size:12px; font-weight:bold; margin-top:8px;");
        layout->addWidget(depLabel);

        bool hasDep = false;
        for (int idx : adjList[stationId]) {
            const Train& t = trains[idx];
            QString toName;
            for (const Station& s : stations)
                if (s.id == t.toId) { toName = s.name; break; }

            auto* card = new QFrame;
            card->setStyleSheet("QFrame{background-color:#212d42; border:1px solid #2e3f5c; border-radius:8px; padding:2px;}");
            auto* cL = new QVBoxLayout(card);
            cL->setContentsMargins(10, 8, 10, 8);
            cL->setSpacing(4);

            auto* row1 = new QLabel(
                QString("<b style='color:#e2e8f0;'>%1</b><span style='color:#7a8caa;'>  →  %2역</span>")
                .arg(t.trainNo).arg(toName));
            row1->setStyleSheet("font-size:12px;");

            auto* row2 = new QLabel(
                QString("<span style='color:#94a3b8;'>⏱ %1분 &nbsp;&nbsp;</span><span style='color:#4ade80;'>₩%2</span>")
                .arg(t.minutes)
                .arg(QLocale(QLocale::Korean).toString(t.price)));
            row2->setStyleSheet("font-size:11px;");

            auto* btn = new QPushButton("예매하기");
            btn->setStyleSheet(
                "QPushButton{background-color:#0369a1;color:white;border:none;border-radius:5px;padding:5px 10px;font-size:11px;font-weight:bold;}"
                "QPushButton:hover{background-color:#0284c7;}"
                "QPushButton:pressed{background-color:#0ea5e9;}");

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
        if (!hasDep) {
            auto* none = new QLabel("출발 열차 없음");
            none->setStyleSheet("color:#4a6fa5; font-size:11px; padding:4px;");
            layout->addWidget(none);
        }
        layout->addStretch();
        hintLabel->setText(QString("선택된 역: %1역").arg(stationName));
    }
};
