/// @file fl_comment.hpp
/// @author Johannes Schneider
/// @brief make use of comments whenever it is neccesary or useful

#ifndef INEXOR_VSCRIPT_COMMENT_HEADER
#define INEXOR_VSCRIPT_COMMENT_HEADER

#include "inexor/flowgraph/nodebase/fl_base.hpp"

namespace inexor {
namespace vscript {

    class CCommentNode : public CScriptNode
    {
        public:

        CCommentNode(vec pos, const char* comment = "This is a comment", const char* name = "");
        ~CCommentNode();
        
        bool OnRelationDragStart();
        bool OnLinkAsChildNodeAttempt(CScriptNode* parent);
        bool OnLinkAsParentNodeAttempt(CScriptNode* child);
    };

};
};

#endif
