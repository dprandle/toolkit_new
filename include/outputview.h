#ifndef OUTPUT_VIEW_H
#define OUTPUT_VIEW_H

#define OUTVIEW_CLRSCRN_ACTION "Clear screen"
#define OUTVIEW_COPY_ACTION "Copy"
#define OUTVIEW_SELECTALL_ACTION "Select all"

// Class Forward Declarations
class QTextEdit;
class QToolBar;
class QAction;

#include <QWidget>

class OutputView : public QWidget
{
	Q_OBJECT

public:
	OutputView(QWidget * parent=NULL);

	~OutputView();

    void init();

	void writeToScreen(const QString & text);

	void clearScreen();

private slots:

	void onClearScreen();

	void onSelectAll();

	void onCopy();

private:
	void _connect();

	QTextEdit * screen;
	QToolBar * topToolBar;
	QAction * clrScreen;
	QAction * selectAll;
	QAction * copy;
};
#endif
