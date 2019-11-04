#include <graph_view.h>
#include <ui_graph_view.h>
#include <mtdebug_print.h>
#include <urho_common.h>
#include <component_widget.h>
#include <toolkit.h>
#include <urho_map_editor.h>
#include <ui_toolkit.h>
#include <editor_selection_controller.h>

//#include <QToolBar>
#include <QLayout>
#include <QToolButton>
#include <QTreeWidget>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>

Graph_View::Graph_View(QWidget * parent)
    : QMainWindow(parent), base_node_(nullptr), ui(new Ui::Graph_View), event_rx(nullptr)
{
    ui->setupUi(this);
    QVBoxLayout * layout = new QVBoxLayout;
    //layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    tw_ = new QTreeWidget;
    tw_->setHeaderHidden(false);
    tw_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tw_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw_->setStyleSheet(GRAPH_TW_STYLE);
    tw_->header()->setStretchLastSection(false);
    tw_->header()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tw_);
    ui->widget_graph->setLayout(layout);

    HashSet<StringHash> events;
    events.Insert(E_SELECTION_CHANGED);
    events.Insert(E_NODENAMECHANGED);
    events.Insert(E_NODEADDED);
    events.Insert(E_NODEREMOVED);
    events.Insert(E_NODECLONED);
    tw_->setDragEnabled(true);

    event_rx = new Urho_Event_Receiver(bbtk_ctxt, events);

    event_rx->push_function(E_SELECTION_CHANGED, this, &Graph_View::handle_selection_change);
    event_rx->push_function(E_NODENAMECHANGED, this, &Graph_View::handle_node_renamed);
    event_rx->push_function(E_NODEADDED, this, &Graph_View::handle_node_added);
    event_rx->push_function(E_NODEREMOVED, this, &Graph_View::handle_node_removed);
    event_rx->push_function(E_NODECLONED, this, &Graph_View::handle_node_cloned);

    connect(tw_->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &Graph_View::selection_changed);
    connect(&bbtk, &Toolkit::urho_init_complete, this, &Graph_View::urho_init_complete);
}

Graph_View::~Graph_View()
{
    delete ui;
    delete event_rx;
}

void Graph_View::set_base_node(Urho3D::Node * node)
{
    base_node_ = node;
    rebuild();
}

void Graph_View::urho_init_complete()
{
    Scene * active_scene = bbtk.ui->map_editor->get_urho_editor()->get_active_scene();
    set_base_node(active_scene);

    auto cats = bbtk_ctxt->GetObjectCategories();
    auto vals = bbtk_ctxt->GetObjectFactories();

    QMenu * menu = new QMenu(this);
    QToolButton * toolButton = new QToolButton();
    toolButton->setText("Create");
    toolButton->setMenu(menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    ui->toolBar->addWidget(toolButton);

    QAction * action_create_empty = new QAction("Empty Node", this);
    QAction * action_create_tile = new QAction("Tile", this);
    QAction * action_create_static_object = new QAction("Static Object", this);
    QAction * action_create_light = new QAction("Light", this);
    QAction * action_create_camera = new QAction("Camera", this);
    menu->addAction(action_create_empty);
    menu->addAction(action_create_tile);
    menu->addAction(action_create_static_object);
    menu->addAction(action_create_light);
    menu->addAction(action_create_camera);

    auto empty_func = [=]() { create_empty(base_node_); };
    auto tile_func = [=]() { create_tile(base_node_); };
    auto static_object_func = [=]() { create_static_obj(base_node_); };
    auto light_func = [=]() { create_light(base_node_); };
    auto camera_func = [=]() { create_camera(base_node_); };

    connect(action_create_empty, &QAction::triggered, empty_func);
    connect(action_create_tile, &QAction::triggered, tile_func);
    connect(action_create_static_object, &QAction::triggered, static_object_func);
    connect(action_create_light, &QAction::triggered, light_func);
    connect(action_create_camera, &QAction::triggered, camera_func);
}

void Graph_View::rebuild()
{
    if (base_node_ == nullptr)
        return;

    tw_->clear();
    tw_->setColumnCount(2);
    QStringList header_labels;
    header_labels.push_back("Name");
    header_labels.push_back("ID");
    tw_->setHeaderLabels(header_labels);
    build_recursively(nullptr, base_node_);
}

void Graph_View::build_recursively(QTreeWidgetItem * parent, Node * node)
{
    QTreeWidgetItem * item = nullptr;
    if (node->GetType() != StringHash("Scene"))
    {
        item = new QTreeWidgetItem;
        item->setText(0, node->GetName().CString());
        item->setData(1, Qt::EditRole, node->GetID());

        if (parent == nullptr)
            tw_->addTopLevelItem(item);
        else
            parent->addChild(item);
    }

    // Create item for this node, then create items for children nodes recursively
    PODVector<Node *> children = node->GetChildren(false);
    for (auto child : children)
        build_recursively(item, child);
}

void Graph_View::create_empty(Urho3D::Node * parent)
{
    parent->CreateChild("New Node");
    rebuild();
}

void Graph_View::create_tile(Urho3D::Node * parent)
{}

void Graph_View::create_static_obj(Urho3D::Node * parent)
{}

void Graph_View::create_light(Urho3D::Node * parent)
{}

void Graph_View::create_camera(Urho3D::Node * parent)
{}

void recurse_select(const HashSet<int> & node_ids, QTreeWidgetItem * item)
{
    if (node_ids.Contains(item->data(1, Qt::EditRole).toInt()))
    {
        QTreeWidgetItem * parent = item->parent();
        if (parent != nullptr && !parent->isExpanded())
            parent->setExpanded(true);
        item->setSelected(true);
    }

    for (int i = 0; i < item->childCount(); ++i)
        recurse_select(node_ids, item->child(i));
}

void Graph_View::handle_selection_change(Urho3D::StringHash event_type,
                                         Urho3D::VariantMap & event_data)
{
    // First disconnect our selection changed signal from the tree view so that its not triggered as we
    // change the selection to reflect the editor selection
    disconnect(tw_->selectionModel(),
               &QItemSelectionModel::selectionChanged,
               this,
               &Graph_View::selection_changed);

    // The currently selected items are passed in from the selection system as a variant vector
    // The node pointers are stored in the variant vector as void pointers so must be casted back
    const VariantVector & selection =
        event_data[SelectionChanged::P_CUR_SELECTION].GetVariantVector();
    // After casting back to a node* we will add the node id to this set
    HashSet<int> node_ids;
    for (int i = 0; i < selection.Size(); ++i)
    {
        Node * nd = static_cast<Node *>(selection[i].GetVoidPtr());
        node_ids.Insert(nd->GetID());
    }

    // Clear the selection in the tree widget
    tw_->selectionModel()->clearSelection();

    // Now recursively go through each item in the tree widget and if the item node id is found in the editor's selection
    // then set the item selected, and if the selected item has a parent make sure the parent gets expanded as well
    for (int i = 0; i < tw_->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem * cur_item = tw_->topLevelItem(i);
        recurse_select(node_ids, cur_item);
    }

    // Now reconnect the selection changed signal to the view so that changing the selection in the tree view will
    // change it in the editor as well
    connect(tw_->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &Graph_View::selection_changed);
}

void Graph_View::handle_node_added(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data)
{
    rebuild();
}

void Graph_View::handle_node_removed(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data)
{
    //rebuild();
}

void Graph_View::handle_node_renamed(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data)
{
    Node * nd = static_cast<Node *>(event_data[NodeNameChanged::P_NODE].GetPtr());
    QString num(QString::number(nd->GetID()));
    auto items = tw_->findItems(num, Qt::MatchExactly, 1);
    if (items.size() == 1)
        items.first()->setText(0, nd->GetName().CString());
}

void Graph_View::handle_node_cloned(Urho3D::StringHash event_type, Urho3D::VariantMap & event_data)
{
    rebuild();
}

void Graph_View::selection_changed()
{
    // Remove the function that handles editor selection changes for now - that way when we set the editor's selection
    // it doesn't keep looping back and calling our function
    event_rx->pop_function(E_SELECTION_CHANGED);

    // Should always be a scene - stored as node though just in case
    if (base_node_->GetType() == StringHash("Scene"))
    {
        Scene * scn = static_cast<Scene *>(base_node_);
        Editor_Selection_Controller * sel_cont = scn->GetComponent<Editor_Selection_Controller>();

        // Now clear the editor's selection and go through the selection from the tree widget - if the item is currently
        // selected then set the item as selected in the editor
        if (sel_cont != nullptr)
        {
            sel_cont->clear_selection();

            QItemSelection selection = tw_->selectionModel()->selection();
            auto iter = selection.begin();
            while (iter != selection.end())
            {
                QModelIndexList indexes = iter->indexes();
                auto index_iter = indexes.begin();
                while (index_iter != indexes.end())
                {
                    int node_id = index_iter->data(Qt::EditRole).toInt();
                    Node * cur_node = scn->GetNode(node_id);
                    sel_cont->add_to_selection(cur_node);
                    ++index_iter;
                }
                ++iter;
            }
        }
    }

    // Now add the editor selection change event handler back in
    event_rx->push_function(E_SELECTION_CHANGED, this, &Graph_View::handle_selection_change);
}