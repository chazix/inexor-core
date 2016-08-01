/// @file fl_area_block.hpp
/// @author Johannes Schneider
/// @brief a block area is defined by position, width, height, and depth

// TODO: make block areas rotatable in at least one dimension

#ifndef INEXOR_VSCRIPT_AREA_BLOCK_HEADER
#define INEXOR_VSCRIPT_AREA_BLOCK_HEADER

#include "inexor/flowgraph/nodebase/fl_base.hpp"

namespace inexor {
namespace vscript {

    class CCubeAreaNode : public CScriptNode
    {
        protected:
            
            float box_width;
            float box_height;
            float box_depth;

        public:

            CCubeAreaNode(vec, float, float, float, const char*, const char*);
            ~CCubeAreaNode();
            
            bool collide(vec p);
            void render_additional();

            bool OnLinkAsChildNodeAttempt(CScriptNode* parent);
            bool OnLinkAsParentNodeAttempt(CScriptNode* child);
    };

};
};

#endif
