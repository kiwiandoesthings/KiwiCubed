#include "Inventory.h"
#include "ModHandler.h"

Inventory::Inventory(std::vector<AssetStringID> slotStringIDs) {
    for (int iterator = 0; iterator < slotStringIDs.size(); iterator++) {
        inventorySlots.insert(std::make_pair(slotStringIDs[iterator], InventorySlot{}));
    }
}

void Inventory::SetSlot(AssetStringID slotStringID, InventorySlot newItem) {
    auto slot = inventorySlots.find(slotStringID);
    if (slot != inventorySlots.end()) {
        inventorySlots[slotStringID] = newItem;
    } else {
        WARN("Could not find slot: \"" + slotStringID.CanonicalName() + "\" in inventory: \"" + inventoryStringID.CanonicalName() + "\"");
    }
}

InventorySlot* Inventory::GetSlot(AssetStringID slotStringID) {
    auto slot = inventorySlots.find(slotStringID);
    if (slot != inventorySlots.end()) {
        return &inventorySlots[slotStringID];
    } else {
        WARN("Could not find slot: \"" + slotStringID.CanonicalName() + "\" in inventory: \"" + inventoryStringID.CanonicalName() + "\"");
    }
}

void Inventory::ClearInventory() {
    for (auto& slot : inventorySlots) {
        slot.second.itemStringID = AssetStringID{};
        slot.second.itemCount = 0;
    }
}

AssetStringID Inventory::GetInventoryStringID() {
    return inventoryStringID;
}