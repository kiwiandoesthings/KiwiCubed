#include "Inventory.h"
#include "ModHandler.h"


Inventory::Inventory() {
    return;
}

Inventory::Inventory(std::vector<AssetStringID> slotStringIDs) {
    for (int iterator = 0; iterator < slotStringIDs.size(); iterator++) {
        inventorySlots.insert(std::make_pair(slotStringIDs[iterator], InventorySlot{AssetStringID{"kiwicubed", "block/air"}, 0}));
    }
}

bool Inventory::AddItem(InventorySlot newItemSlot) {
    for (auto iterator = inventorySlots.begin(); iterator != inventorySlots.end(); iterator++) {
        InventorySlot& itemSlot = iterator->second;
        if (itemSlot.itemStringID.assetName == "block/air") {
            itemSlot.itemStringID = newItemSlot.itemStringID;
            itemSlot.itemCount++;
            return true;
        }
        if (itemSlot.itemStringID == newItemSlot.itemStringID && itemSlot.itemCount + newItemSlot.itemCount <= 64) {
            itemSlot.itemCount++;
            return true;
        }
    }
    return false;
}

void Inventory::SetSlot(AssetStringID slotStringID, InventorySlot newItem) {
    auto slot = inventorySlots.find(slotStringID);
    if (slot != inventorySlots.end()) {
        inventorySlots[slotStringID] = newItem;
    } else {
        CRITICAL("Could not find slot: \"" + slotStringID.CanonicalName() + "\" in inventory: \"" + inventoryStringID.CanonicalName() + "\"");
        psnip_trap();
    }
}

InventorySlot* Inventory::GetSlot(AssetStringID slotStringID) {
    auto slot = inventorySlots.find(slotStringID);
    if (slot != inventorySlots.end()) {
        return &inventorySlots[slotStringID];
    } else {
        CRITICAL("Could not find slot: \"" + slotStringID.CanonicalName() + "\" in inventory: \"" + inventoryStringID.CanonicalName() + "\"");
        psnip_trap();
        return nullptr;
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