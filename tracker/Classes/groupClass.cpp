#include "../headers.h"

//: Declaration
// class Group {
// public:

//     Group(string groupId, string adminId);

//     void printValue();
//     string adminId;
//     string groupId;
//     unordered_set <string> pendingRequests;
//     unordered_set <string> participants;
//     unordered_map <string, File> fielsOfGroup;  
// };

Group::Group(string groupId, string adminId) {
    this->groupId = groupId;
    this->adminId = adminId;
}


void Group::printValue(){
    cout << "groupId : " << setw(15) << groupId << ", adminId : " << setw(15)  << adminId  << "," << endl  
        << "pendingRequests : ";
    
    for(auto it: pendingRequests){
        cout << it << " ";
    }
    cout << endl;
    cout << "participants : ";
    for(auto it: participants){
        cout << it << " "; 
    }
    cout << endl;
    for(auto it: filesOfGroup){
        it.second.printValue();
        cout << endl;
    }
    return;
}

