#include "BidirectedGraph.hpp"
#include <queue>

vector<uint64_t> BidirectedGraph::get_reachable_nodes(uint64_t id){
    return reachable_nodes.at(id);
}

void BidirectedGraph::add_edge(uint64_t id1, uint64_t id2, bool from_left, bool to_right){
    BidirectedEdge tempedge(id1, id2);
    tempedge.from_left = from_left;
    tempedge.to_right = to_right;
  
    edges.emplace(make_pair(id1, vector<BidirectedEdge>()));
    edges.emplace(make_pair(id2, vector<BidirectedEdge>()));
    edges[id1].push_back(tempedge);
    edges[id2].push_back(tempedge);
}

bool BidirectedGraph::is_acyclic(){
    return true;
}

void BidirectedGraph::populate_reachable_nodes(){
    unordered_map<uint64_t, vector<BidirectedEdge> >::iterator mapiter = edges.begin();
    while(mapiter!=edges.end()){
        mod_BFS(mapiter->first);
        ++mapiter;
    }
}


// lowercase g means entering from left
// uppercase G means entering from right
void BidirectedGraph::mod_BFS(uint64_t id){
    if(reachable_nodes.find(id)==reachable_nodes.end()){
        reachable_nodes.emplace(id, vector<uint64_t>());
    }
    unordered_map<uint64_t, char > colormap = unordered_map<uint64_t, char >();
    queue<uint64_t> queue = ::queue<uint64_t>();
    queue.push(id);
    colormap.emplace(make_pair(id, 'g'));
    while(queue.size()!=0){
        uint64_t currnode = queue.front();
        queue.pop();
        char color = colormap.at(currnode);
        vector<BidirectedEdge>::iterator veciter = edges.at(id).begin();
        while(veciter!=edges.at(id).end()){
            if(color=='g'){
                if(veciter->id1==currnode && !veciter->from_left && colormap.find(veciter->id2)==colormap.end()){
                    reachable_nodes.at(id).push_back(veciter->id2);
                    queue.push(veciter->id2);
                    colormap.emplace(veciter->id2, veciter->to_right ? 'G' : 'g');
                }
                else if(veciter->id2==currnode && veciter->to_right && colormap.find(veciter->id1)==colormap.end()){
                    reachable_nodes.at(id).push_back(veciter->id1);
                    queue.push(veciter->id1);
                    colormap.emplace(veciter->id1, veciter->from_left ? 'g' : 'G');
                }
            }
            else if(color=='G'){
                if(veciter->id1==currnode && veciter->from_left && colormap.find(veciter->id2)==colormap.end()){
                    reachable_nodes.at(id).push_back(veciter->id2);
                    queue.push(veciter->id2);
                    colormap.emplace(veciter->id2, veciter->to_right ? 'G' : 'g');
                }
                else if(veciter->id2==currnode && !veciter->to_right && colormap.find(veciter->id1)==colormap.end()){
                    reachable_nodes.at(id).push_back(veciter->id1);
                    queue.push(veciter->id1);
                    colormap.emplace(veciter->id1, veciter->from_left ? 'g' : 'G');
                }
            }
            ++veciter;
        }
        colormap.at(currnode) = 'b';
    }
    queue = ::queue<uint64_t>();
    colormap = unordered_map<uint64_t, char>();
    queue.push(id);
    colormap.emplace(make_pair(id, 'G'));
    while(queue.size()!=0){
        uint64_t currnode = queue.front();
        queue.pop();
        char color = colormap.at(currnode);
        vector<BidirectedEdge>::iterator veciter = edges.at(id).begin();
        while(veciter!=edges.at(id).end()){
            if(color=='g'){
                if(veciter->id1==currnode && !veciter->from_left && colormap.find(veciter->id2)==colormap.end()){
                    reachable_nodes.at(id).push_back(veciter->id2);
                    queue.push(veciter->id2);
                    colormap.emplace(veciter->id2, veciter->to_right ? 'G' : 'g');
                }
                else if(veciter->id2==currnode && veciter->to_right && colormap.find(veciter->id1)==colormap.end()){
                    reachable_nodes.at(id).push_back(veciter->id1);
                    queue.push(veciter->id1);
                    colormap.emplace(veciter->id1, veciter->from_left ? 'g' : 'G');
                }
            }
            else if(color=='G'){
                if(veciter->id1==currnode && veciter->from_left && colormap.find(veciter->id2)==colormap.end()){
                    reachable_nodes.at(id).push_back(veciter->id2);
                    queue.push(veciter->id2);
                    colormap.emplace(veciter->id2, veciter->to_right ? 'G' : 'g');
                }
                else if(veciter->id2==currnode && !veciter->to_right && colormap.find(veciter->id1)==colormap.end()){
                    reachable_nodes.at(id).push_back(veciter->id1);
                    queue.push(veciter->id1);
                    colormap.emplace(veciter->id1, veciter->from_left ? 'g' : 'G');
                }
            }
            ++veciter;
        }
        colormap.at(currnode) = 'b';
    }
}

void BidirectedGraph::print_reachable_nodes(){
    unordered_map<uint64_t, vector<uint64_t> >::iterator reachable = reachable_nodes.begin();
    while(reachable!=reachable_nodes.end()){
        vector<uint64_t>::iterator nodeiter = reachable->second.begin();
        printf("Printing path connected nodes from %llu:\n", reachable->first);
        while(nodeiter!=reachable->second.end()){
            printf("%llu\n", *nodeiter);
            nodeiter++;
        }
        reachable++;
    }
}