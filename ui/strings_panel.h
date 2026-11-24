#pragma once
#include <QtWidgets>
#include <QtConcurrent>
#include <QFutureWatcher>

struct TraceString {
    qint64 offset;
    QString value;
};

inline void openStringsPanel(QWidget *parent)
{
    const auto path = QFileDialog::getOpenFileName(parent, QObject::tr("Open file"));
    if (path.isEmpty()) return;
    auto *dialog = new QDialog(parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("StringsTrace - Strings");
    dialog->resize(900, 620);
    auto *layout = new QVBoxLayout(dialog);
    auto *search = new QLineEdit(dialog);
    search->setPlaceholderText(QObject::tr("Filter strings"));
    auto *table = new QTableWidget(0, 2, dialog);
    table->setHorizontalHeaderLabels({QObject::tr("Offset"), QObject::tr("String")});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto *status = new QLabel(QObject::tr("Reading file..."), dialog);
    layout->addWidget(search);
    layout->addWidget(table);
    layout->addWidget(status);
    auto *watcher = new QFutureWatcher<QVector<TraceString>>(dialog);
    QObject::connect(search, &QLineEdit::textChanged, dialog, [table](const QString &query) {
        for (int i = 0; i < table->rowCount(); ++i)
            table->setRowHidden(i, !table->item(i, 1)->text().contains(query, Qt::CaseInsensitive));
    });
    QObject::connect(watcher, &QFutureWatcher<QVector<TraceString>>::finished, dialog, [=] {
        const auto rows = watcher->result();
        table->setUpdatesEnabled(false);
        table->setRowCount(rows.size());
        for (int i = 0; i < rows.size(); ++i) {
            table->setItem(i, 0, new QTableWidgetItem(QString::number(rows[i].offset, 16)));
            table->setItem(i, 1, new QTableWidgetItem(rows[i].value));
            table->setRowHidden(i, !rows[i].value.contains(search->text(), Qt::CaseInsensitive));
        }
        table->setUpdatesEnabled(true);
        status->setText(QObject::tr("%1 strings (preview limit: 100000)").arg(rows.size()));
    });
    watcher->setFuture(QtConcurrent::run([path] {
        QVector<TraceString> rows;
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return rows;
        QByteArray value;
        qint64 offset = 0, start = 0;
        auto flush = [&] {
            if (value.size() >= 4) rows.append({start, QString::fromLatin1(value)});
            value.clear();
        };
        while (!file.atEnd() && rows.size() < 100000) {
            const auto block = file.read(65536);
            if (block.isEmpty()) break;
            for (unsigned char c : block) {
                if (c >= 32 && c <= 126) {
                    if (value.isEmpty()) start = offset;
                    value.append(char(c));
                    if (value.size() >= 65536) flush();
                } else flush();
                ++offset;
                if (rows.size() >= 100000) break;
            }
        }
        if (rows.size() < 100000) flush();
        return rows;
    }));
    dialog->show();
}
