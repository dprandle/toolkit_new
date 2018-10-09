#pragma once

#include <QWidget>
#include <urho_editor.h>

namespace Urho3D
{
class Context;
}


class Scene_View : public QWidget
{
    Q_OBJECT

    friend class Toolkit;

  public:
    Scene_View(QWidget * parent = nullptr);

    ~Scene_View();

    template<class EditorType>
    void init_editor(Urho3D::Context * context)
    {
        if (me_ != nullptr)
            release_editor();
        me_ = new EditorType(context, (void *)winId());
        me_->update_gl_context();
        _start_timer();
    }

    template<class EditorType>
    EditorType * get_editor()
    {
        return static_cast<EditorType*>(me_);
    }

    Urho_Editor * get_editor()
    {
        return me_;
    }

    void release_editor();

    void enterEvent(QEvent * e) override;

    void resizeEvent(QResizeEvent * e) override;

    void mousePressEvent(QMouseEvent * e) override;

    void mouseReleaseEvent(QMouseEvent * e) override;

    void mouseMoveEvent(QMouseEvent * e) override;

    void wheelEvent(QWheelEvent * e) override;

    void keyPressEvent(QKeyEvent * e) override;

    void keyReleaseEvent(QKeyEvent * e) override;

  public slots:

    void run();

  private:
    void _start_timer();

    Urho_Editor * me_;
};