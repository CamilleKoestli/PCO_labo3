#ifndef SELLER_H
#define SELLER_H

#include <QString>
#include <QStringBuilder>
#include <pcosynchro/pcomutex.h>
#include <map>
#include <vector>
#include "costs.h"

enum class ItemType {
    PatientSick, PatientHealed, Syringe, Pill, Scalpel, Thermometer, Stethoscope, Nothing
};

int getCostPerUnit(ItemType item);
QString getItemName(ItemType item);

enum class EmployeeType {Supplier, Nurse, Doctor};

EmployeeType getEmployeeThatProduces(ItemType item);
int getEmployeeSalary(EmployeeType employee);

class Seller {
public:
    /**
     * @brief Seller
     * @param money money money !
     */
    Seller(int money, int uniqueId) : money(money), uniqueId(uniqueId) {}

    /**
     * @brief getItemsForSale
     * @return The list of items for sale
     */
    virtual std::map<ItemType, int> getItemsForSale() = 0;

    /**
     * @brief Fonction permettant d'acheter des ressources au vendeur
     * @param Le type de resource à acheter
     * @param Nombre de ressources voulant être achetées
     * @return La facture : côut de la resource * le nombre, 0 si indisponible
     */
    virtual int send(ItemType what, int qty, int bill) = 0;
    virtual int request(ItemType what, int qty) = 0;

    /**
     * @brief chooseRandomSeller
     * @param sellers
     * @return Returns a random seller from the sellers vector
     */
    static Seller* chooseRandomSeller(std::vector<Seller*>& sellers);

    /**
     * @brief getRandomItemFromStock
     * @return Returns a random ItemType from the stocks
     */
    ItemType getRandomItemFromStock() {
        if (stocks.empty()) {
            throw std::runtime_error("Stock is empty.");
        }

        auto it = stocks.begin();
        std::advance(it, rand() % stocks.size());

        return it->first;
    }

    /**
     * @brief Chooses a random item type from an items for sale map
     * @param itemsForSale
     * @return Returns the item type
     */
    static ItemType chooseRandomItem(std::map<ItemType, int>& itemsForSale);

    int getFund() { return money; }

    int getUniqueId() { return uniqueId; }

protected:
    PcoMutex mutex;
    /**
     * @brief stocks : Type, Quantité
     */
    std::map<ItemType, int> stocks;
    int money;
    int uniqueId;
};

#endif // SELLER_H
