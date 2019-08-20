#ifndef PARAMDIALOG_H
#define PARAMDIALOG_H

#include <QDialog>

namespace Ui {
class ParamDialog;
}

class ParamDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ParamDialog(QWidget *parent = 0);
    ~ParamDialog();
    inline bool changed() { return m_changed; };
private slots:
    void dbClicked();
    void videoClicked();
    void photoClicked();
    void accept();
    
private:
    Ui::ParamDialog *ui;
    bool m_changed;
};

#endif // PARAMDIALOG_H
