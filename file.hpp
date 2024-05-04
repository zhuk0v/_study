#pragma once

#include <memory>
#include <string>
#include <vector>

#include "graphic_objects/graphic_objects_base.hpp"

namespace edt {

    // ----------------------------------------------------------------

    /// @brief Объявление
    class File;

    namespace file_cmd {
        // ----------------------------------------------------------------

        /**
         * @brief Базовый класс для всех команд которые необходимо выполнить над файлом
         *
         */
        class CommandBase {
        protected:
            std::shared_ptr<File> m_file;
        public:
            /**
             * @brief Монтирование экземпляра класса
             *
             */
            CommandBase(std::shared_ptr<File> file) : m_file(file) {}
            /**
             * @brief Размонтирование экземпляра класса
             *
             */
            virtual ~CommandBase() = default;

            /**
             * @brief Метод для выполнения команды
             *
             */
            virtual void execute() = 0;
        };

        // ----------------------------------------------------------------

        class CreateGraphicsObjectCommand : public CommandBase {
        private:
            std::string graphics_object_type;
            std::string graphics_object_name;
        public:
            /**
             * @brief Монтирование экземпляра класса
             *
             */
            CreateGraphicsObjectCommand(std::shared_ptr<File> file, std::string obj_type, std::string obj_name) : CommandBase(file),
                graphics_object_type(obj_type),
                graphics_object_name(obj_name) {}

            void execute() override;
        };

        // ----------------------------------------------------------------

        class RemoveGraphicsObjectCommand : public CommandBase {
        private:
            std::string graphics_object_type;
            std::string graphics_object_name;
        public:
            /**
             * @brief Монтирование экземпляра класса
             *
             */
            RemoveGraphicsObjectCommand(std::shared_ptr<File> file, std::string obj_type, std::string obj_name) : CommandBase(file),
                graphics_object_type(obj_type),
                graphics_object_name(obj_name) {}

            void execute() override;
        };


    } // namespace file_cmd

    // ----------------------------------------------------------------

    template <typename T>
    class Observable {
    protected:
        std::weak_ptr<T> m_observer;
    public:
        /**
         * @brief Добавление наблюдателя
         *
         * @param observer
         */
        void AddObserver(std::shared_ptr<T> observer) {
            m_observer = observer;
        }
    };

    // ----------------------------------------------------------------

    /// @brief Объявление
    class FileViewer;

    class FileModel : public Observable<FileViewer> {
    private:

        std::vector<std::shared_ptr<GraphicObjectsBase>> m_objs;

    public:
        /**
         * @brief Монтирование экземпляра класса
         *
         */
        FileModel() = default;
        /**
         * @brief Размонтирование экземпляра класса
         *
         */
        ~FileModel() = default;

        void Add(std::shared_ptr<GraphicObjectsBase> obj);

        void Remove(std::string type, std::string name);

    private:
        void NotifyObservers();
    };

    // ----------------------------------------------------------------

    class FileViewer {
    public:
        /**
         * @brief Монтирование экземпляра класса
         *
         */
        FileViewer() = default;
        /**
         * @brief Размонтирование экземпляра класса
         *
         */
        ~FileViewer() = default;

        void Update(std::vector<std::shared_ptr<GraphicObjectsBase>>& objs) {
            // Отображение объектов
        }
    };

    // ----------------------------------------------------------------

    /// @brief Объявление
    class FileVisitor;


    class File : public std::enable_shared_from_this<File> {
    private:
        std::string m_filename;

        std::shared_ptr<FileModel> m_model;
        std::shared_ptr<FileViewer> m_viewer;

    public:
        /**
         * @brief Монтирование экземпляра класса
         *
         */
        File(std::string filename);

        /**
         * @brief Размонтирование экземпляра класса
         *
         */
        ~File() = default;

        /**
         * @brief Имя объекта
         * 
         * @return auto 
         */
        auto Name(){
            return m_filename;
        }

        /**
         * @brief Модель файла
         * 
         */
        auto GetModel() {
            return m_model;
        }

        /**
         * @brief Метод приёма посетителя
         *
         */
        void Accept(std::shared_ptr<FileVisitor> ptr_visitor);
    };

} // namespace edt