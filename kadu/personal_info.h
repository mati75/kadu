#ifndef PERSONAL_INFO_H
#define PERSONAL_INFO_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include "libgadu.h"

/**
	Dialog umożliwiający zarządzanie własnymi danymi w katalogu
	publicznym
**/
class PersonalInfoDialog : public QDialog
{
	Q_OBJECT

	private:
		QLineEdit* NicknameEdit;
		QLineEdit* NameEdit;
		QLineEdit* SurnameEdit;
		QComboBox* GenderCombo;
		QLineEdit* BirthyearEdit;
		QLineEdit* CityEdit;
		QLineEdit* FamilyNameEdit;
		QLineEdit* FamilyCityEdit;
		enum DialogState {READY, READING, WRITTING};
		DialogState State;
		int seq;

	private slots:
		void OkButtonClicked();

	protected:
		virtual void closeEvent(QCloseEvent * e);

	public:
		PersonalInfoDialog(QDialog *parent=0, const char *name=0);

	public slots:
		void fillFields(gg_pubdir50_t res);
};

#endif
