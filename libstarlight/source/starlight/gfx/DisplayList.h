#pragma once
#include "starlight/_global.h"

#include <vector>
#include <memory>
#include <type_traits>
#include <functional>

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"
#include "starlight/datatypes/Color.h"

namespace starlight {
    namespace gfx {
        enum DLNodeType {
            Quads,
            Func
        };
        
        class DLNode {
        public:
            virtual DLNodeType Type() = 0;
            virtual void Apply(Vector2 offset) = 0;
        };
        
        class DLNode_Quads : public DLNode {
            std::vector<std::pair<VRect, VRect>> rects;
        public:
            static constexpr DLNodeType NodeType = DLNodeType::Quads;
            DLNodeType Type() { return NodeType; }
            void Apply(Vector2 offset) override;
            inline void Add(const VRect& quad, const VRect& uv) { rects.push_back(std::pair<VRect, VRect>(quad.IntSnap(), uv)); }
        };
        
        class DLNode_Func : public DLNode {
            std::function<void(Vector2)> func;
        public:
            DLNode_Func(std::function<void(Vector2)> func) : func(func) { }
            static constexpr DLNodeType NodeType = DLNodeType::Func;
            DLNodeType Type() { return NodeType; }
            void Apply(Vector2 offset) override;
        };
        
        class DisplayList {
        private:
            std::vector<std::shared_ptr<DLNode>> nodes;// = std::vector<std::shared_ptr<DLNode>>(8);
        public:
            Vector2 offset;
            
            DisplayList() { }
            ~DisplayList() { }
            
            void Run(Vector2 offset);
            
            inline void AddNode(std::shared_ptr<DLNode> node) { nodes.push_back(std::static_pointer_cast<DLNode>(node)); }
            template <typename T, typename... Ps>
            inline std::shared_ptr<T> CreateNode(Ps... args) {
                static_assert(std::is_base_of<DLNode, T>::value, "T is not a subtype of DLNode");
                
                nodes.push_back(std::make_shared<T>(args...));
                return std::static_pointer_cast<T>(nodes.back());
            }
            template <typename T>
            inline std::shared_ptr<T> GetLastNode(bool create = false) {
                static_assert(std::is_base_of<DLNode, T>::value, "T is not a subtype of DLNode");
                if (nodes.empty() || nodes.back()->Type() != T::NodeType) {
                    if (!create) return nullptr;
                    nodes.push_back(std::make_shared<T>());
                }
                return std::static_pointer_cast<T>(nodes.back());
            }
            
            void AddQuad(const VRect& quad, const VRect& uv);
            void AddFunc(std::function<void(Vector2)> func);
            
            bool Valid();
            
            //
        };
    }
}

