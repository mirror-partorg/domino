#include "paramdialog.h"
#include "ui_paramdialog.h"
#include "config.h"
#include <QSettings>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include "qsqlconnectiondialog.h"

ParamDialog::ParamDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParamDialog),
    m_changed(false)
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    ui->setupUi(this);

    ui->videoEdit->setText(settings.value("piece-path").toString());
    QString defaultPath;
    defaultPath = QDir(QDesktopServices::storageLocation ( QDesktopServices::DocumentsLocation )).filePath("Snapshot");
    ui->photoEdit->setText(settings.value("snapshot-path", defaultPath).toString());
    ui->styleEdit->setText(settings.value("style-sheet", "").toString());

    for ( int i=0; i < 10; i++ ) {
        QStringList line;
        line = settings.value(QString("cams-%1").arg(i)).toStringList();
        if ( line.isEmpty() )
            break;
        int j;
        j = 0;
        foreach (QString s, line) {
            QTableWidgetItem *item;
            item = ui->camsTable->item(i,j);
            if ( !item ) {
                item = new QTableWidgetItem(s);
                ui->camsTable->setItem(i,j, item);
            }
            else
                item->setText(s);
            j++;
        }
    }
    //ui->camsEdit->setText(val);
    connect(ui->dbButton, SIGNAL(clicked()), this, SLOT(dbClicked()));
    connect(ui->videoButton, SIGNAL(clicked()), this, SLOT(videoClicked()));
    connect(ui->photoButton, SIGNAL(clicked()), this, SLOT(photoClicked()));
}


ParamDialog::~ParamDialog()
{
    delete ui;
}
void ParamDialog::dbClicked()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    if ( QSqlConnectionDialog::sqlConnectionDialog(&settings) )
        m_changed = true;
}

void ParamDialog::videoClicked()
{
    QFileDialog d;
    QString piecesPath;
    piecesPath = d.getExistingDirectory(NULL, "Выберите директорию видео", ui->videoEdit->text());
    if (! piecesPath.isEmpty() )
        ui->videoEdit->setText(piecesPath);
}

void ParamDialog::photoClicked()
{
    QFileDialog d;
    QString piecesPath;
    piecesPath = d.getExistingDirectory(NULL, "Выберите директорию для фото", ui->photoEdit->text());
    if ( !piecesPath.isEmpty() )
        ui->photoEdit->setText(piecesPath);
}

void ParamDialog::accept()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    if ( settings.value("piece-path").toString() != ui->videoEdit->text() ) {
        settings.setValue("piece-path", ui->videoEdit->text());
        m_changed = true;
    }
    if ( settings.value("snapshot-path").toString() != ui->photoEdit->text() ) {
        settings.setValue("snapshot-path", ui->photoEdit->text());
    }
    if ( settings.value("style-sheet").toString() != ui->styleEdit->text() ) {
        settings.setValue("style-sheet", ui->styleEdit->text());
    }
    for ( int i=0; i < 10; i++ ) {
        QStringList line;
        line.clear();
        for ( int j=0; j < 10; j++  ) {
            QTableWidgetItem *item;
            item = ui->camsTable->item(i,j);
            if ( item && !item->text().isNull() && !item->text().isEmpty() )
                line.append(item->text());
        }
        if ( settings.value(QString("cams-%1").arg(i)).toStringList() !=  line ) {
            settings.setValue(QString("cams-%1").arg(i), line);
            m_changed = true;
        }
    }
    QDialog::accept();
}
