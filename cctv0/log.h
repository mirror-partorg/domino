#ifndef LOG_H
#define LOG_H

#define SHOW_ERROR(a) {if ( QMessageBox::critical(NULL, tr("Ошибка"), a.append(tr("\nЗавершить работу?")), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes ) exit(1); }
#define SHOW_ERROR_EXIT(a) { QMessageBox::critical(NULL, tr("Ошибка"), a, QMessageBox::Ok); exit(1); }
#define SHOW_WARNING(a) QMessageBox::warning(NULL, tr("Предупреждение"), a, QMessageBox::Ok)

#endif // LOG_H
