#ifndef WALLET_H
#define WALLET_H

class Wallet {
public:
    int balance;
    Wallet() : balance(100) {} // เริ่มต้น 100 coin
    void addMoney(int amount) { balance += amount; }
    bool spendMoney(int amount) {
        if (balance >= amount) { balance -= amount; return true; }
        return false;
    }
};

#endif
