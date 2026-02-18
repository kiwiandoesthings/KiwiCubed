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
        Inventory();
        Inventory(std::vector<AssetStringID> slotStringIDs);
        void Delete();

        bool AddItem(InventorySlot newItemSlot);
        bool AddItem(InventorySlot newItemSlot, AssetStringID slotStringID);

        void SetSlot(AssetStringID slotStringID, InventorySlot newItem);
        InventorySlot* GetSlot(AssetStringID slotStringID);

        void ClearInventory();

        AssetStringID GetInventoryStringID();
    private:
        AssetStringID inventoryStringID;

        std::map<AssetStringID, InventorySlot> inventorySlots;
};