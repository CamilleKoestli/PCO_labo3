#include "hospital.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

IWindowInterface *Hospital::interface = nullptr;

Hospital::Hospital(int uniqueId, int fund, int maxBeds)
    : Seller(fund, uniqueId), maxBeds(maxBeds), currentBeds(0),
      nbHospitalised(0), nbFree(0) {
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(
        uniqueId, "Hospital Created with " + QString::number(maxBeds) + " beds");

    std::vector<ItemType> initialStocks = {
        ItemType::PatientHealed,
        ItemType::PatientSick
    };

    for (const auto &item: initialStocks) {
        stocks[item] = 0;
    }
}

int Hospital::request(ItemType what, int qty) {
    // TODO
    if (qty <= 0)
        return 0;

    mutex.lock();
    bool requestSuccessful = false;

    if (what == ItemType::PatientSick && currentBeds >= qty) {
        currentBeds -= qty;
        money -= getEmployeeSalary(EmployeeType::Nurse); // Paie l'infirmier pour chaque patient
        interface->updateFund(uniqueId, money);
        requestSuccessful = true;
    }

    mutex.unlock();

    return requestSuccessful ? qty : 0;
}

void Hospital::freeHealedPatient() {
    // TODO
    mutex.lock();
    if (currentBeds < 0) {
        currentBeds--;
        nbFree++;
        money -= getEmployeeSalary(EmployeeType::Nurse); // Paie l'infirmier pour chaque patient
        interface->updateFund(uniqueId, money);

        interface->consoleAppendText(uniqueId, "A healed patient has been discharged from the hospital.");
        interface->updateStock(uniqueId, &stocks);
    }
    mutex.unlock();
}

void Hospital::transferPatientsFromClinic() {
    // TODO
    for (auto &clinic: clinics) {
        int healedPatients = clinic->request(ItemType::PatientHealed, 1);

        if (healedPatients > 0) {
            mutex.lock();

            int nurseSalary = getEmployeeSalary(EmployeeType::Nurse);
            if (currentBeds < maxBeds) {
                currentBeds++;
                nbHospitalised++;
                money -= nurseSalary; // Paie l'infirmier pour chaque patient
                interface->updateFund(uniqueId, money);

                interface->consoleAppendText(uniqueId, "Transferred a healed patient from the clinic to the hospital.");
                interface->updateStock(uniqueId, &stocks);
                mutex.unlock();
                break; // stop le transfert après avoir add un patient
            } else {
                mutex.unlock(); // Déverrouille si le transfert ne peut pas se faire
                if (currentBeds >= maxBeds) {
                    interface->consoleAppendText(uniqueId, "No available beds for transferring a patient.");
                } else if (money < nurseSalary) {
                    interface->consoleAppendText(
                        uniqueId, "Insufficient funds to pay the nurse for transferring a patient.");
                }
            }
        }
    }
}

int Hospital::send(ItemType it, int qty, int bill) {
    // TODO
    // Si le patient est malade, vérification lit disponible et fonds suffisants
    mutex.lock();
    bool transactionSuccessful = false; // permet de factoriser le code et de ne faire qu'un seul unlock

    if (it == ItemType::PatientSick) {
        int totalCost = bill + getEmployeeSalary(EmployeeType::Nurse) * qty;
        if (qty > 0 && currentBeds + qty <= maxBeds && money >= totalCost) {
            money -= totalCost;
            currentBeds += qty;

            transactionSuccessful = true;
        }
    }
    // Si le patient est guéri, réduire les lits occupés
    else if (it == ItemType::PatientHealed) {
        if (qty > 0 && currentBeds >= qty) {
            currentBeds -= qty;
            transactionSuccessful = true;
        }
    }

    mutex.unlock();

    if (transactionSuccessful) {
        interface->updateFund(uniqueId, money);
    }
    // retourne le status de la transaction
    return transactionSuccessful ? bill : 0;
}

void Hospital::run() {
    if (clinics.empty()) {
        std::cerr << "You have to assign clinics to a hospital before launching its routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Hospital routine");

    // Boucle principale de la routine de l'hôpital
    while (!PcoThread::thisThread()->stopRequested()) {
        // Transfert des patients guéris des cliniques à l'hôpital
        transferPatientsFromClinic();

        // Libération des patients guéris ayant terminé leur convalescence
        freeHealedPatient();

        // Mise à jour de l'interface pour afficher les fonds et les stocks
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);

        // Simule un délai d'attente
        interface->simulateWork();
    }

    interface->consoleAppendText(uniqueId, "[STOP] Hospital routine");
}

int Hospital::getAmountPaidToWorkers() {
    return nbHospitalised * getEmployeeSalary(EmployeeType::Nurse);
}

int Hospital::getNumberPatients() {
    return stocks[ItemType::PatientSick] + stocks[ItemType::PatientHealed] +
           nbFree;
}

std::map<ItemType, int> Hospital::getItemsForSale() { return stocks; }

void Hospital::setClinics(std::vector<Seller *> clinics) {
    this->clinics = clinics;

    for (Seller *clinic: clinics) {
        interface->setLink(uniqueId, clinic->getUniqueId());
    }
}

void Hospital::setInterface(IWindowInterface *windowInterface) {
    interface = windowInterface;
}
