#include <QTimer>

#include <Urho3D/Core/Context.h>

#include <map_editor.h>
#include <scene_view.h>
#include <input_translator.h>

Scene_View::Scene_View(QWidget * parent) : QWidget(parent), me_(nullptr)
{
    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);
}

Scene_View::~Scene_View()
{
    release_editor();
}

void Scene_View::_start_timer()
{
    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(run()));
    timer->start();
}

void Scene_View::release_editor()
{
    delete me_;
    me_ = nullptr;
}

void Scene_View::run()
{
    me_->update(hasFocus());
}

void Scene_View::resizeEvent(QResizeEvent * e)
{
    if (me_ != nullptr)
    {
        me_->make_current();
        me_->input_translator_->qt_window_resize(e);
    }
    QWidget::resizeEvent(e);
}

void Scene_View::enterEvent(QEvent * e)
{
    if (me_ != nullptr)
    {
        me_->make_current();
    }
    QWidget::enterEvent(e);
}

void Scene_View::mousePressEvent(QMouseEvent * e)
{
    if (me_ != nullptr)
    {
        me_->make_current();
        me_->input_translator_->qt_mouse_press(e);
    }
    QWidget::mousePressEvent(e);
}

void Scene_View::mouseReleaseEvent(QMouseEvent * e)
{
    if (me_ != nullptr)
    {
        me_->make_current();
        me_->input_translator_->qt_mouse_release(e);
    }
    QWidget::mouseReleaseEvent(e);
}

void Scene_View::mouseMoveEvent(QMouseEvent * e)
{
    if (me_ != nullptr)
    {
        me_->make_current();
        me_->input_translator_->qt_mouse_move(e);
    }
    QWidget::mouseMoveEvent(e);
}

void Scene_View::wheelEvent(QWheelEvent * e)
{
    if (me_ != nullptr)
    {
        me_->make_current();
        me_->input_translator_->qt_mouse_wheel(e);
    }
    QWidget::wheelEvent(e);
}

void Scene_View::keyPressEvent(QKeyEvent * e)
{
    if (me_ != nullptr)
    {
        me_->make_current();
        me_->input_translator_->qt_key_press(e);
    }
    QWidget::keyPressEvent(e);
}

void Scene_View::keyReleaseEvent(QKeyEvent * e)
{
    if (me_ != nullptr)
    {
        me_->make_current();
        me_->input_translator_->qt_key_release(e);
    }
    QWidget::keyReleaseEvent(e);
}
