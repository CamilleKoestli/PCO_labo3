#include "seller.h"
#include <algorithm>
#include <random>
#include <cassert>

Seller *Seller::chooseRandomSeller(std::vector<Seller *> &sellers) {
    assert(sellers.size());
    std::vector<Seller*> out;
    std::sample(sellers.begin(), sellers.end(), std::back_inserter(out),
            1, std::mt19937{std::random_device{}()});
    return out.front();
}

ItemType Seller::chooseRandomItem(std::map<ItemType, int> &itemsForSale) {
    if (!itemsForSale.size()) {
        return ItemType::Nothing;
    }
    std::vector<std::pair<ItemType, int> > out;
    std::sample(itemsForSale.begin(), itemsForSale.end(), std::back_inserter(out),
            1, std::mt19937{std::random_device{}()});
    return out.front().first;
}

int getCostPerUnit(ItemType item) {
    switch (item) {
        case ItemType::Syringe : return SYRINGUE_COST;
        case ItemType::Pill : return PILL_COST;
        case ItemType::Scalpel : return SCALPEL_COST;
        case ItemType::Thermometer : return THERMOMETER_COST;
        case ItemType::Stethoscope : return STETHOSCOPE_COST;
        case ItemType::PatientHealed : return HEALING_COST;
        case ItemType::PatientSick : return TRANSFER_COST;
        default : return 0;
    }
}

QString getItemName(ItemType item) {
    switch (item) {
        case ItemType::Syringe : return "Syringe";
        case ItemType::Pill : return "Pill";
        case ItemType::Scalpel : return "Scalpel";
        case ItemType::Thermometer : return "Thermometer";
        case ItemType::Stethoscope : return "Stethoscope";
        case ItemType::PatientHealed : return "Patient Healed";
        case ItemType::PatientSick : return "Patient Sick";
        case ItemType::Nothing : return "Nothing";
        default : return "???";
    }
}

EmployeeType getEmployeeThatProduces(ItemType item) {
    switch (item) {
        case ItemType::Syringe : /* fallthrough */
        case ItemType::Pill : /* fallthrough */
        case ItemType::Scalpel : /* fallthrough */
        case ItemType::Thermometer : /* fallthrough */
        case ItemType::PatientSick : /* fallthrough */
        case ItemType::Stethoscope : return EmployeeType::Supplier;
        case ItemType::PatientHealed : return EmployeeType::Doctor;
        default : return EmployeeType::Nurse;
    }
}

int getEmployeeSalary(EmployeeType employee) {
    switch (employee) {
        case EmployeeType::Nurse : return NURSE_COST;
        case EmployeeType::Supplier : return SUPPLIER_COST;
        case EmployeeType::Doctor : return DOCTOR_COST;
        default : return 0;
    }
}
