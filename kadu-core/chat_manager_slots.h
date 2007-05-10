#ifndef KADU_CHAT_MANAGER_SLOTS_H
#define KADU_CHAT_MANAGER_SLOTS_H

#include <qobject.h>
#include <qcolor.h>
#include <qfont.h>

class QComboBox;
class QLineEdit;

/**
	\class ChatSlots
	\brief Sloty do obs逝gi okna konfiguracji
**/
class ChatManagerSlots :public QObject
{
	Q_OBJECT

	private:
		/**
			\fn void updatePreview()
			Od鈍ie瞠nie wygl康u
		**/
		void updatePreview();

	public:
		/**
			\fn ChatManagerSlots(QObject* parent=0, const char* name=0)
			Konstruktor obiektu
			\param parent rodzic obiektu
			\param name nazwa obiektu
		**/
		ChatManagerSlots(QObject* parent=0, const char* name=0);

	public slots:
		/**
			\fn void onCreateTabChat()
			Slot jest wywo造wany gdy otwierana jest zakladka Chat
		**/
		void onCreateTabChat();

		/**
			\fn void onCreateTabLook()
			Slot jest wywo造wany gdy otwierana jest zakladka Wyglad
		**/
		void onCreateTabLook();

		/**
			\fn void onApplyTabChat()
			Slot jest wywo造wany gdy zamykane jest okno konfiguracji
		**/
		void onApplyTabChat();

		/**
			\fn void onApplyTabLook()
			Slot jest wywo造wany gdy zamykane jest okno konfiguracji
		**/
		void onApplyTabLook();

		/**
			\fn void chooseEmoticonsStyle(int index)
			Slot jest wywo造wany gdy wybierany jest styl ikonek
			\param index nr stylu emotikonek
		**/
		void chooseEmoticonsStyle(int index);

		/**
			\fn void onPruneChat(bool toggled)
			Slot jest wywo造wany gdy wybierane jest czy otwierane okno
			ma by� pusty czy nie
			\param toggled
		**/
		void onPruneChat(bool toggled);

		/**
			\fn void onFoldLink(bool toggled)
			Slot jest wywo造wany gdy w陰czono/wy陰czono zawijanie link闚,
			aby uaktualni� podgl康
			\param toggled
		**/
		void onFoldLink(bool toggled);

		/**
			\fn void onBlockClose(bool toggled)
			Slot jest wywo造wany gdy wybierany jest maksymalny czas
			blokowania zamkni璚ia okna po otrzymaniu wiadomo軼i
			\param toggled
		**/
		void onBlockClose(bool toggled);

		/**
			\fn void onRemoveHeaders(bool toggled)
			Slot jest wywo造wany gdy w陰czono/wy陰czono wy鈍ietlanie nag堯wk闚,
			aby uaktualni� podgl康
			\param toggled
		**/
		void onRemoveHeaders(bool toggled);


		/**
			\fn void onRemoveServerTime(bool toggled)
			Slot jest wywo造wany gdy w陰czono/wy陰czono usuwanie czasu serwera
			\param toggled
		**/
		void onRemoveServerTime(bool toggled);

		/**
			\fn void onChatThemeChanged(const QString& name)
			Slot jest wywo造wany po zmianie stylu okna Chat,
			aby uaktualni� podgl康
			\param name nazwa stylu
		**/
		void onChatThemeChanged(const QString& name);

		/**
			\fn void chooseColor(const char* name, const QColor& color)
			Slot jest wywo造wany gdy zmieniono kolor,
			aby uaktualni� podgl康
			\param name nazwa koloru
			\param color kolor
		**/
		void chooseColor(const char* name, const QColor& color);

		/**
			\fn void chooseFont(const char* name, const QFont& font)
			Slot jest wywo造wany gdy zmieniono czcionke,
			aby uaktualni� podgl康
			\param name nazwa czcionki
			\param font czcionka
		**/
		void chooseFont(const char* name, const QFont& font);

		/**
			\fn void chooseBackgroundFile()
			Slot jest wywo造wany aby wybra� obraz t豉 dla okna Chat
		**/
		void chooseBackgroundFile();

		/**
			\fn void findAndSetWebBrowser(int selectedBrowser)
			Slot jest wywo造wany gdy wybrano z listy przegl康ark�
			\param selectedBrowser nr przegl康arki
		**/
		void findAndSetWebBrowser(int selectedBrowser);

		/**
			\fn void findAndSetBrowserOption(int selectedOption)
			Slot jest wywo造wany gdy aktywowano pole do wpisania opcji
			\param selectedOption wybrana opcja
		**/
		void findAndSetBrowserOption(int selectedOption);

		/**
			\fn static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
			Slot ustawia przegl康arki wraz z opcjami
			\param browserCombo
			\param browserOptionsCombo
			\param browserPath
		**/
		static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);

		/**
			\fn static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
			Slot jest wywo造wany gdy wybrano z listy przegl康ark�
			\param selectedBrowser
			\param browserCombo
			\param browserOptionsCombo
			\param browserPath
		**/
		static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);

		/**
			\fn static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser)
			Slot ustawia przegl康arki wraz z opcjami
			\param selectedOption
			\param browsePathEdit
			\param chosenBrowser
		**/
		static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser);

		/**
			\fn void setMailPath(int mailNumber)
			Slot ustawia odpowiedni wpis dla wybranego klienta pocztowego
			\param mailNumber numer wybranego klienta
		**/
		void setMailPath(int mailNumber);

		/**
			\fn static void setMailClients(QComboBox *mailClient)
			Slot tworzy list� program闚 pocztowych
			\param mailClient
		**/
		static void setMailClients(QComboBox *mailClient);

		/**
			\fn void findMailClient(const unsigned int mailNumber)
			Slot pr鏏uje znale潭 plik binarny wybranego klienta pocztowego
			\param mailNumber numer wybranego klienta
		**/
		void findMailClient(const unsigned int mailNumber);
};
#endif
