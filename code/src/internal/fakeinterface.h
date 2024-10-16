#ifndef FAKEINTERFACE_H
#define FAKEINTERFACE_H

#include <vector>
#include <string>
#include <map>
#include "seller.h"
#include "iwindowinterface.h"

class FakeInterface : public IWindowInterface {
public:
    void consoleAppendText(unsigned int consoleId, QString text) override {
        log.push_back("Console: " + text.toStdString());
    }

    void updateFund(unsigned int uniqueId, unsigned fund) override {
        funds[uniqueId] = fund;
    }

    void updateStock(unsigned int id, std::map<ItemType, int>* stocks) override {
        if (stocks) {
            latestStocks[id] = *stocks;
        }
    }

    void simulateWork() override {
        // Implémentation fictive
    }

    void setLink(int from, int to) override {
        // Implémentation fictive
    }

    void setUtils(Utils* utils) override {
        // Implémentation fictive
    }

    [[nodiscard]]
    std::string getLastConsoleMessage() const {
        return log.empty() ? "" : log.back();
    }

    [[nodiscard]]
    int getFundFor(unsigned int uniqueId) const {
        auto it = funds.find(uniqueId);
        return (it != funds.end()) ? it->second : 0;
    }

    [[nodiscard]]
    const std::map<ItemType, int>& getStockFor(unsigned int uniqueId) const {
        return latestStocks.at(uniqueId);
    }

private:
    std::vector<std::string> log;  
    std::map<unsigned int, unsigned int> funds;     
    std::map<unsigned int, std::map<ItemType, int>> latestStocks;  
};

#endif // FAKEINTERFACE_H
