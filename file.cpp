#include "file.hpp"

#include "file_visitor.hpp"

#include "graphic_objects/circle.hpp"

// ----------------------------------------------------------------
void edt::file_cmd::CreateGraphicsObjectCommand::execute() {
    if (graphics_object_type == "circle") {
        m_file->GetModel()->Add(std::make_shared<Circle>(graphics_object_name));
    }
}

// ----------------------------------------------------------------
void edt::file_cmd::RemoveGraphicsObjectCommand::execute() {
    m_file->GetModel()->Remove(graphics_object_type, graphics_object_name);
}

// ----------------------------------------------------------------
void edt::FileModel::Add(std::shared_ptr<GraphicObjectsBase> obj) {
    // Обработка нового объекта, сохранение
    m_objs.push_back(obj);
    // ...

    // Если после всей логики объект нужно вывести на экран, то необходимо оповестить наблюдателей
    NotifyObservers();
}

// ----------------------------------------------------------------
void edt::FileModel::Remove(std::string type, std::string name) {
    // Удаление объекта из вектора
    for (auto it = m_objs.begin(); it < m_objs.end(); it++) {
        if ((*it)->Name() == name) {
            m_objs.erase(it);
        }
    }
    // ...

    // Если после всей логики объект нужно вывести на экран, то необходимо оповестить наблюдателей
    NotifyObservers();
}

// ----------------------------------------------------------------
void edt::FileModel::NotifyObservers() {
    if (auto observer = m_observer.lock()) {
        // 
        observer->Update(m_objs);
    }
}

// ----------------------------------------------------------------
edt::File::File(std::string filename) : m_filename(filename) {
    m_model = std::make_unique<FileModel>();
    m_viewer = std::make_shared<FileViewer>();

    m_model->AddObserver(m_viewer);
}

// ----------------------------------------------------------------
void edt::File::Accept(std::shared_ptr<FileVisitor> ptr_visitor) {
    ptr_visitor->Visit(shared_from_this());
}