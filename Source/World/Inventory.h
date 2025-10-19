#pragma once

#include "AssetManager.h"


struct InventorySlot {
    AssetStringID itemStringID;
    unsigned char itemCount;

    InventorySlot() : itemStringID(AssetStringID{}), itemCount(0) {}
    InventorySlot(unsigned char itemCount) : itemCount(itemCount) {}
    InventorySlot(AssetStringID itemStringID, unsigned char itemCount) : itemStringID(itemStringID), itemCount(itemCount) {}
};


class Inventory {
    public:
        Inventory(std::vector<AssetStringID> slotStringIDs);
        void Delete();

        void SetSlot(AssetStringID slotStringID, InventorySlot newItem);
        InventorySlot* GetSlot(AssetStringID slotStringID);

        void ClearInventory();

        AssetStringID GetInventoryStringID();
    private:
        AssetStringID inventoryStringID;

        std::unordered_map<AssetStringID, InventorySlot> inventorySlots;
};