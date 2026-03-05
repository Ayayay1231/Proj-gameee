#include<iostream>
#include<string>
using namespace std ;

//void cal(int ,int );


int main(){
    string name[]={"acha","play","u","dome","MEE"};
    int id[] = {1,2,3,4,5};
    double w[] ={49,89,65,44,56};
    bool found = false ;
    string key;
    cin >> key;


    for(int i=0;i<5;i++){

        if(key == name[i]){
            cout << id[i] << " " << name[i] << " " << w[i];
            found = true ;

        }
    }


    if(found == false){
        cout << "Not found" ;
    }
    return 0;
}