#include "inexor/flowgraph/flowgraph.hpp"
#include "inexor/fpsgame/game.hpp"
#include "inexor/flowgraph/sleep/fl_sleep.hpp"
#include "inexor/flowgraph/timer/fl_timer.hpp"
#include "inexor/flowgraph/comments/fl_comment.hpp"

// experimental
#include "inexor/flowgraph/memory/fl_membase.hpp"

// functions
#include "inexor/flowgraph/functions/fl_functionbase.hpp"
#include "inexor/flowgraph/functions/fl_functions.hpp"

// areas
#include "inexor/flowgraph/areas/block/fl_area_block.hpp"
#include "inexor/flowgraph/areas/sphere/fl_area_sphere.hpp"
#include "inexor/flowgraph/areas/cone/fl_area_cone.hpp"
#include "inexor/flowgraph/areas/cylinder/fl_area_cylinder.hpp"

// operators
#include "inexor/flowgraph/operators/increment/fl_increment.hpp"

// data types
#include "inexor/flowgraph/memory/integer/fl_mem_integer.hpp"

// geometry
#include "inexor/geom/geom.hpp"
#include "inexor/geom/curves/bezier/bezier.hpp"


// use the engine's selection model
extern selinfo sel, lastsel, savedsel;
extern bool editmode;
extern int gridsize;

enum VSCRIPT_ENTITY_BOX_ORIENTATION
{
    INEXOR_VSCRIPT_BOX_NO_INTERSECTION = -1,
    INEXOR_VSCRIPT_BOX_LEFT,
    INEXOR_VSCRIPT_BOX_RIGHT,
    INEXOR_VSCRIPT_BOX_FRONT,
    INEXOR_VSCRIPT_BOX_BACK,
    INEXOR_VSCRIPT_BOX_BOTTOM,
    INEXOR_VSCRIPT_BOX_TOP
};

namespace inexor {
namespace vscript {

    // TODO: make CVisualScriptSystem a singleton class (optional?)
    // create an instance of the visual scripting system
    CVisualScriptSystem vScript3D;

    CVisualScriptSystem::CVisualScriptSystem() 
    {
        selection_blocked_by_geometry = false;
    }

    CVisualScriptSystem::~CVisualScriptSystem() 
    {
        // TODO: we have to clear the dynamicly allocated memory!
        for(unsigned int i=0; i<nodes.size(); i++) delete(nodes[i]);
        nodes.clear();
    }

    // creates a node and allocates memory for it
    CScriptNode* CVisualScriptSystem::add_node(INEXOR_VSCRIPT_NODE_TYPE type, int parameter_count, ...)
    {
        // this memory must be deleted afterwards!
        CScriptNode* created_node = nullptr;

        vec target = vec(sel.o.x,sel.o.y,sel.o.z);
        vec offset = vec(gridsize/2,gridsize/2,gridsize/2);
        target.add(offset);

        // convert parameters to parameter list
        va_list parameters;
        std::vector<std::string> arguments;
        va_start(parameters, parameter_count);
        for(unsigned int i=0; i<parameter_count; i++)
        {
            arguments.push_back(va_arg(parameters, char *));
        }
        va_end(parameters);

        switch(type)
        {
            case INEXOR_VSCRIPT_NODE_TYPE_TIMER:
            {
                /// convert parameters form const string to unsigned int
                unsigned int interval   = atoi(arguments[0].c_str());
                unsigned int startdelay = atoi(arguments[1].c_str());
                unsigned int limit      = atoi(arguments[2].c_str());
                unsigned int cooldown   = atoi(arguments[3].c_str());
                const char* name        = arguments[4].c_str();
                const char* comment     = arguments[5].c_str();
                
                if(0 == interval) interval = 1000;

                /// TODO...
                INEXOR_VSCRIPT_TIME_FORMAT timer_format = INEXOR_VSCRIPT_TIME_FORMAT_MILISECONDS;
                
                created_node = new CTimerNode(target, interval, startdelay, limit, cooldown, name, comment, timer_format);
                sync_all_timers();
                break;
            }

            case INEXOR_VSCRIPT_NODE_TYPE_COMMENT:
            {
                if(!strlen(arguments[0].c_str()))
                {
                    conoutf(CON_DEBUG, "[3DVS-comments] error: empty string as comment!");
                    break;
                }
                created_node = new CCommentNode(target, arguments[0].c_str());
                break;
            }

            case INEXOR_VSCRIPT_NODE_TYPE_FUNCTION:
            {
                if (!strlen(arguments[0].c_str()))
                {
                    conoutf(CON_DEBUG, "[3DVS-functions] error: no function name speficied!");
                    break;
                }

                switch(atoi(arguments[0].c_str()))
                {
                    case INEXOR_VSCRIPT_FUNCTION_CONOUTF:
                    {
                        if (!strlen(arguments[1].c_str()))
                        {
                            conoutf(CON_DEBUG, "[3DVS-functions-conoutf] error: no output text specified!");
                            break;
                        }
                        // add a console text output function
                        created_node = new CFunctionConoutfNode(target, arguments[1].c_str() );
                        break;
                    }
                    case INEXOR_VSCRIPT_FUNCTION_PLAYSOUND:
                    {
                        if (!strlen(arguments[1].c_str()))
                        {
                            conoutf(CON_DEBUG, "[3DVS-functions-playsound] error: sound to play specified!");
                            break;
                        }
                        // add a sound player function
                        created_node = new CFunctionPlaysoundNode(target, arguments[1].c_str() );
                        break;
                    }
                }
                break;
            }

            case INEXOR_VSCRIPT_NODE_TYPE_SLEEP:
            {
                unsigned int wait_time   = atoi(arguments[0].c_str());
                if(0 == wait_time) wait_time = 500;
                created_node = new CSleepNode(target, wait_time); 
                break;
            }

            case INEXOR_VSCRIPT_NODE_TYPE_AREA_BOX:
            {
                float area_width  = atof(arguments[0].c_str());
                float area_height = atof(arguments[1].c_str());
                float area_depth  = atof(arguments[2].c_str());
                created_node = new CCubeAreaNode(target, area_width, area_height, area_depth, "box", "");
                break;
            }

            case INEXOR_VSCRIPT_NODE_TYPE_AREA_SPHERE:
            {
                float radius = atof(arguments[0].c_str());
                created_node = new CSphereAreaNode(target, radius, "sphere", "");
                break;
            }

            case INEXOR_VSCRIPT_NODE_TYPE_AREA_CONE:
            {
                float height = atof(arguments[0].c_str());
                float radius = atof(arguments[1].c_str());
                created_node = new CConeAreaNode(target, height, radius, "cone", "");
                break;
            }

            case INEXOR_VSCRIPT_NODE_TYPE_AREA_CYLINDER:
            {
                float height = atof(arguments[0].c_str());
                float radius = atof(arguments[1].c_str());
                created_node = new CCylinderAreaNode(target, height, radius, "cylinder", "");
                break;
            }

            case INEXOR_VSCRIPT_NODE_TYPE_MEMORY_INTEGER:
            {
                created_node = new CMemIntegerNode(target, true, true, atoi(arguments[0].c_str()), "integer", "can have positive values (also null and negative)");
                break;
            }
        }

        if(nullptr != created_node) nodes.push_back(created_node);
        return created_node;
        // TODO: delete created_node at program exit!
    }
    
    void CVisualScriptSystem::remove_node(CScriptNode* node)
    {
        // TODO: implement "remove node"!
    }

    // calculates the distance from editor to entity
    void CVisualScriptSystem::update_drag_n_drop()
    {
        if(nullptr != selected_node)
        {
            static float selection_dist = 0.0f;
            if(!distance_calculated)
            {
                selection_dist = selected_node->pos.dist(game::player1->o);
                distance_calculated = true;
            }
            
            vec camdir_normalized = camdir;
            camdir_normalized.normalize();
            selected_node->pos = game::player1->o + camdir_normalized.mul(selection_dist);
            selected_node->pos_changed = true;
        }
    }

    // updates positions of relation curves and renders them
    void CVisualScriptSystem::update_relation_linker()
    {
        if(nullptr != relation_start_node)
        {
            static float selected_dist = 0.0f;
            static bool calculated = false;
            if(!calculated)
            {
                selected_dist = relation_drag_start.dist(game::player1->o);
                calculated=true;
            }

            vec camdir_normalized = camdir;
            camdir_normalized.normalize();
            vec drag_target_pos = game::player1->o+camdir_normalized.mul(selected_dist);

            // render line            
            start_rendering();
            glBegin(GL_LINES);
            glLineWidth(5.0f);
            gle::color(vec::hexcolor(INEXOR_VSCRIPT_COLOR_TIMER));
            
            float box_center_offset = boxsize/2;
            glVertex3f(drag_target_pos.x,drag_target_pos.y,drag_target_pos.z);
            glVertex3f(relation_drag_start.x+box_center_offset ,relation_drag_start.y+box_center_offset ,relation_drag_start.z+box_center_offset );

            glLineWidth(1.0f);
            glEnd();
            end_rendering();
        }
    }

    // adds a new relation between the nodes and a new curve for the relation
    void CVisualScriptSystem::connect_nodes(CScriptNode *from, CScriptNode *to)
    {
        // store parent and chil in each other's vectors
        to->parents.push_back(from);
        from->children.push_back(to);

        // create a new relation curve
        SNodeRelation newcurve;
        newcurve.from = from;
        newcurve.to = to;

        from->relations.push_back(newcurve);
        from->pos_changed = true;

        conoutf(CON_DEBUG, "[3DVS-node-linker] linked parent %s with child %s.", from->node_name.c_str(), to->node_name.c_str());
    }

    void CVisualScriptSystem::disconnect_nodes(CScriptNode *from, CScriptNode *to)
    {
        // TODO: implement disconnect nodes!
    }

    // makes sure all the node's rules for linking are followed
    void CVisualScriptSystem::validate_new_relation(CScriptNode *from, CScriptNode *to)
    {
        if(nullptr != hovered_node)
        {
            // do not link nodes to itself!
            if(hovered_node != relation_start_node)
            {
                // check if nodes allow to be linked
                if(from->OnLinkAsParentNodeAttempt(to) && to->OnLinkAsChildNodeAttempt(from))
                {
                    connect_nodes(from, to);
                }
                else
                {
                    conoutf(CON_DEBUG, "[3DVS-node-linker] could not link parent %s with child %s.", from->node_name.c_str(), to->node_name.c_str());
                }
            }
            else
            {
                conoutf(CON_DEBUG,"[3DVS-node-linker] a node can't be linked to itself!");
            }
        }
    }

    void CVisualScriptSystem::update_input(int key, bool isdown)
    {
        // TODO: why the minus?
        switch( - key)
        {
            // left mouse button for dragging relations
            case SDL_BUTTON_LEFT:
            {
                if(!dragging_new_relation && isdown && !selection_blocked_by_geometry)
                {
                    // iterate through all nodes and check for box intersection
                    // TODO: store them and SORT by distance!
                    for(unsigned int i=0; i<nodes.size(); i++)
                    {
                        float dist = 0.0f;
                        int orient = INEXOR_VSCRIPT_BOX_NO_INTERSECTION;
                        vec p = nodes[i]->pos;

                        if(rayboxintersect(p, vec(boxsize), camera1->o, camdir, dist, orient))
                        {
                            // does this node allow relations to be dragged?
                            if(nodes[i]->OnRelationDragStart())
                            {
                                relation_drag_start = p;
                                relation_start_node = nodes[i];
                                dragging_new_relation = true;
                            }
                        }
                    }
                }
                if(dragging_new_relation && !isdown) // stop dragging
                {
                    // check if new relation should be formed
                    validate_new_relation(relation_start_node, hovered_node);
                    relation_start_node = nullptr;
                    dragging_new_relation = false;
                }
                break;
            }

            /// right mouse button for moving
            case SDL_BUTTON_RIGHT:
            {
                if(isdown && nullptr != hovered_node)
                {
                    /// key pressed
                    selected_node = hovered_node;
                    moving_entity = true;
                }
                else 
                {
                    /// key released
                    if(nullptr != selected_node)
                    {
                        moving_entity = false;
                        selected_node->pos_changed = false;
                        selected_node = nullptr;
                        distance_calculated = false;
                    }
                }
                break;
            }
        }
    }

    void CVisualScriptSystem::sync_all_timers()
    {
        for(unsigned int i=0; i<nodes.size(); i++)
        {
            if(INEXOR_VSCRIPT_NODE_TYPE_TIMER == nodes[i]->type) nodes[i]->reset();
        }
    }

    void CVisualScriptSystem::delete_all_nodes()
    {
        nodes.clear();
    }

    void CVisualScriptSystem::run()
    {
        for(unsigned int i = 0; i < nodes.size(); i++) nodes[i]->recursion_counter = 0;

        for(unsigned int i=0; i<nodes.size(); i++)
        {
            nodes[i]->this_time = SDL_GetTicks();
            if(INEXOR_VSCRIPT_NODE_TYPE_TIMER == nodes[i]->type) nodes[i]->in();
        }
    }

    void CVisualScriptSystem::start_rendering()
    {
        notextureshader->set();
        gle::enablevertex();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        enablepolygonoffset(GL_POLYGON_OFFSET_LINE);
    }

    void CVisualScriptSystem::end_rendering()
    {
        disablepolygonoffset(GL_POLYGON_OFFSET_LINE);
        gle::clearvbo();
        gle::clearebo();
        gle::disablevertex();
    }

    void CVisualScriptSystem::render_nodes()
    {
        hovered_node = nullptr;
        
        for(unsigned int i=0; i<nodes.size(); i++) 
        {
            float dist = 0.0f;
            int orient = INEXOR_VSCRIPT_BOX_NO_INTERSECTION;

            rayboxintersect(nodes[i]->pos, vec(boxsize), camera1->o, camdir, dist, orient);
            nodes[i]->selected = (orient != INEXOR_VSCRIPT_BOX_NO_INTERSECTION);

            // render a 200ms long color effect once its activated
            if( (nodes[i]->this_time - nodes[i]->last_time) < INEXOR_VSCRIPT_ACTIVE_NODE_TIMER_INTERVAL)
            {
                nodes[i]->box_color = INEXOR_VSCRIPT_COLOR_TRIGGERED;
            }
            else 
            {
                nodes[i]->box_color = nodes[i]->default_box_color;
            }

            nodes[i]->render(orient, selection_blocked_by_geometry);
        }

        // which node is selected?
        for(unsigned int i=0; i<nodes.size(); i++)
        {
            if(nodes[i]->selected && nullptr != nodes[i]) hovered_node = nodes[i];
        }
    }

    void CVisualScriptSystem::render_debug_rays()
    {
        glBegin(GL_LINES);
        gle::color(vec::hexcolor(INEXOR_VSCRIPT_COLOR_DEBUG_RAY));
        glLineWidth(10.0f);
        for(unsigned int h=0; h<rays.size(); h++)
        {
            glVertex3f(rays[h].pos.x,rays[h].pos.y,rays[h].pos.z);
            glVertex3f(rays[h].target.x,rays[h].target.y,rays[h].target.z);
        }
        glLineWidth(1.0f);
        glEnd();
    }

    void CVisualScriptSystem::render_node_relations()
    {
        if(!nodes.size()) return;

        for(unsigned int i=0; i<nodes.size(); i++)
        {   
            for(unsigned int e = 0; e < nodes[i]->children.size(); e++)
            {
                SNodeRelation *tmp = &nodes[i]->relations[e];

                // do we have to recompute the cache?
                if(nodes[i]->pos_changed || nodes[i]->children[e]->pos_changed)
                {
                    tmp->curve.ClearAllPoints();
            
                    // add additional interpolation data between the points
                    // to generate a nice and smooth curve
                    vec t = nodes[i]->pos;
                    vec n = nodes[i]->children[e]->pos;
                    vec interpol1 = vec(t.x, t.y, t.z - 40.0f);
                    vec interpol2 = vec(n.x, n.y, n.z + 40.0f);

                    t.x += boxsize/2;
                    t.y += boxsize/2;
                    n.x += boxsize/2;
                    n.y += boxsize/2;
                    n.z += boxsize;

                    tmp->curve.AddParameterPoint(t);
                    tmp->curve.AddParameterPoint(interpol1);
                    tmp->curve.AddParameterPoint(interpol2);
                    tmp->curve.AddParameterPoint(n);
                    
                    tmp->curve.ComputeCache();
                }

                glLineWidth(2.0f);
                glBegin(GL_LINES);

                // render a 200ms long color effect once its activated
                if((nodes[i]->this_time - nodes[i]->last_time) < INEXOR_VSCRIPT_ACTIVE_NODE_TIMER_INTERVAL)
                {
                    gle::color(vec::hexcolor(INEXOR_VSCRIPT_COLOR_TRIGGERED));
                }
                else
                {
                    gle::color(vec::hexcolor(INEXOR_VSCRIPT_COLOR_TIMER));
                }

                for(unsigned int h=0; h<tmp->curve.GetCachedPointsSize() -1; h++)
                {
                    geom::SCustomOutputPoint t = tmp->curve.GetPoint_ByIndex(h);
                    geom::SCustomOutputPoint n = tmp->curve.GetPoint_ByIndex(h+1);
                    glVertex3f(t.pos.x, t.pos.y, t.pos.z);
                    glVertex3f(n.pos.x, n.pos.y, n.pos.z);
                }

                glEnd();
                glLineWidth(1.0f);
            }
        }
    }
    
    void deleteallnodes()
    {
        vScript3D.delete_all_nodes();
    }
    COMMAND(deleteallnodes, "");
    
    void vs_timer(const char* interval)
    {
        vScript3D.add_node(INEXOR_VSCRIPT_NODE_TYPE_TIMER, 7, interval, "0", interval, "0", "timer1", "this is a comment", "0");
    }
    COMMAND(vs_timer, "s");

    void vs_conoutf(const char* text)
    {
        vScript3D.add_node(INEXOR_VSCRIPT_NODE_TYPE_FUNCTION, 2, "0", text);
    }
    COMMAND(vs_conoutf,"s");

    void vs_sleep(const char *interval)
    {
        vScript3D.add_node(INEXOR_VSCRIPT_NODE_TYPE_SLEEP, 1, interval);
    }
    COMMAND(vs_sleep, "s");

    void vs_comment(const char *comment)
    {
        vScript3D.add_node(INEXOR_VSCRIPT_NODE_TYPE_COMMENT, 1, comment);
    }
    COMMAND(vs_comment, "s");

    void vs_memory(const char *value)
    {
        vScript3D.add_node(INEXOR_VSCRIPT_NODE_TYPE_MEMORY_INTEGER, 1, value);
    }
    COMMAND(vs_memory, "s");
    
    void vs_box(const char* w, const char* h, const char* d)
    {
        vScript3D.add_node(INEXOR_VSCRIPT_NODE_TYPE_AREA_BOX, 3, w, h, d);
    }
    COMMAND(vs_box, "ssss");

    void vs_sphere(const char* radius)
    {
        vScript3D.add_node(INEXOR_VSCRIPT_NODE_TYPE_AREA_SPHERE, 1, radius);
    }
    COMMAND(vs_sphere, "s");

    void vs_cone(const char* height, const char* radius)
    {
        vScript3D.add_node(INEXOR_VSCRIPT_NODE_TYPE_AREA_CONE, 2, height, radius);
    }
    COMMAND(vs_cone, "ss");


    void vs_cylinder(const char* height, const char* radius)
    {
        vScript3D.add_node(INEXOR_VSCRIPT_NODE_TYPE_AREA_CYLINDER, 2, height, radius);
    }
    COMMAND(vs_cylinder, "ss");

};
};