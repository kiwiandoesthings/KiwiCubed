#pragma once
#include <limits>
#include <vector>
#include <cassert>


namespace siv
{
    /** Alias the differentiate between IDs and index.
     * An id allows to access the data through the index vector and is associated with the same object until its erased.
     * An index is simply the current position of the object in the data vector and may change with deletions.
     */
    using id = uint64_t;

    static constexpr id InvalidID = std::numeric_limits<id>::max();

    /// Forward declaration
    template<typename TObjectType>
    class vector;

    /** A standalone struct allowing to access an object without the need to have a reference to the containing vector.
     *
     * @tparam TObjectType The type of the object
     */
    /** Standalone object to access an object
     *
     * @tparam TObjectType The object's type
     */
    template<typename TObjectType>
    class handle
    {
    public:
        /// Default constructor
        handle() = default;
        /// Constructor
        handle(id indetifier, id validity_id, vector<TObjectType>* vector)
            : m_id{indetifier}
            , m_validity_id{validity_id}
            , m_vector{vector}
        {}

        /// Pointer-like access to the underlying object
        TObjectType* operator->()
        {
            return &(*m_vector)[m_id];
        }

        /// Const pointer-like access to the object
        TObjectType const* operator->() const
        {
            return &(*m_vector)[m_id];
        }

        /// Dereference operator
        TObjectType& operator*()
        {
            return (*m_vector)[m_id];
        }

        /// Dereference constant operator
        TObjectType const& operator*() const
        {
            return (*m_vector)[m_id];
        }

        /// Returns the id of the associated object
        [[nodiscard]]
        id getID() const
        {
            return m_id;
        }

        /** Bool operator to test against the validity of the reference
         *
         * @return false if uninitialized or if the object has been erased from the vector, true otherwise
         */
        explicit operator bool() const
        {
            return isValid();
        }

        /** Check if the reference is associated with a vector and has a correct validity id
         *
         * @return false if uninitialized or if the object has been erased from the vector, true otherwise
         */
        [[nodiscard]]
        bool isValid() const
        {
            return m_vector && m_vector->isValid(m_id, m_validity_id);
        }

    private:
        /// The id of the object.
        id                   m_id          = 0;
        /// The validity id of the object at the time of creation. Used to check the validity of the handle.
        id                   m_validity_id = 0;
        /// A raw pointer to the vector containing the object associated with this handle
        vector<TObjectType>* m_vector      = nullptr;

        /// Used to perform debug checks
        friend class vector<TObjectType>;
    };

    /** A vector that provide stable IDs when adding objects.
     * These IDs will still allow to access their associated objects even after inserting of removing other objects.
     * This comes at the cost of a small overhead because of an additional indirection.
     *
     * @tparam TObjectType The type of the objects to be stored in the vector. It has to be movable.
     */
    template<typename TObjectType>
    class vector
    {
    public:
        vector() = default;

        /** Copies the provided object at the end of the vector
         *
         * @param object The object to copy
         * @return The id to retrieve the object
         */
        id push_back(const TObjectType& object)
        {
            const id indetifier = getFreeSlot();
            m_data.push_back(object);
            return indetifier;
        }

        /** Constructs an object in place
         *
         * @tparam TArgs Constructor arguments types
         * @param args Constructor arguments
         * @return The id to retrieve the object
         */
        template<typename... TArgs>
        id emplace_back(TArgs&&... args)
        {
            const id indetifier = getFreeSlot();
            m_data.emplace_back(std::forward<TArgs>(args)...);
            return indetifier;
        }

        /** Removes the object from the vector
         *
         * @param indetifier The id of the object to remove
         */
        void erase(id indetifier)
        {
            // Fetch relevant info
            const id data_id      = m_indexes[indetifier];
            const id last_data_id = m_data.size() - 1;
            const id last_id      = m_metadata[last_data_id].rid;
            // Update validity id
            ++m_metadata[data_id].validity_id;
            // Swap the object to delete with the object at the end
            std::swap(m_data[data_id], m_data[last_data_id]);
            std::swap(m_metadata[data_id], m_metadata[last_data_id]);
            std::swap(m_indexes[indetifier], m_indexes[last_id]);
            // Destroy the object
            m_data.pop_back();
        }

        /** Removes the object from the vector
         *
         * @param idx The index in the data vector of the object to remove
         */
        void eraseViaData(uint32_t idx)
        {
            erase(m_metadata[idx].rid);
        }

        /** Removes the object referenced by the handle from the vector
         *
         * @param handle The handle referencing the object to remove
         */
        void erase(const handle<TObjectType>& handle)
        {
            // Ensure the handle is from this vector
            assert(handle.m_vector == this);
            // Ensure the object hasn't already been erased
            assert(handle.isValid());
            erase(handle.getID());
        }

        /** Return the index in the data vector of the object referenced by the provided id
         *
         * @param indetifier The id to find the data index of
         * @return The index in the data vector associated with the id
         */
        [[nodiscard]]
        uint64_t getDataIndex(id indetifier) const
        {
            return m_indexes[indetifier];
        }

        /** Access the object reference by the provided id
         *
         * @param indetifier The object's id
         * @return A reference to the object
         */
        TObjectType& operator[](id indetifier)
        {
            return m_data[m_indexes[indetifier]];
        }

        /** Access the object reference by the provided id
         *
         * @param indetifier The object's id
         * @return A constant reference to the object
         */
         TObjectType const& operator[](id indetifier) const
        {
            return m_data[m_indexes[indetifier]];
        }

        /// Returns the number of objects in the vector
        [[nodiscard]]
        size_t size() const
        {
            return m_data.size();
        }

        /// Tells if the vector is currently empty
        [[nodiscard]]
        bool empty() const
        {
            return m_data.empty();
        }

        /// Returns the vector's capacity (i.e. the number of allocated slots in the vector)
        [[nodiscard]]
        size_t capacity() const
        {
            return m_data.capacity();
        }

        /** Creates a handle pointing to the provided id
         *
         * @param indetifier The id of the object
         * @return A handle to the object
         */
        handle<TObjectType> createhandle(id indetifier)
        {
            /* Ensure the object is valid. If the data index is greater than the current size
             * it means that it has been swapped and removed. */
            assert(getDataIndex(indetifier) < size());
            return {indetifier, m_metadata[m_indexes[indetifier]].validity_id, this};
        }

        /** Creates a handle to an object using its position in the data vector
         *
         * @param idx The index of the object in the data vector
         * @return A handle to the object
         */
        handle<TObjectType> createhandleFromData(uint64_t idx)
        {
            /* Ensure the object is valid. If the data index is greater than the current size
             * it means that it has been swapped and removed. */
            assert(idx < size());
            return {m_metadata[idx].rid, m_metadata[idx].validity_id, this};
        }

        /** Checks if the provided object is still valid considering its last known validity id
         *
         * @param indetifier The id of the object
         * @param validity_id The last known validity id
         * @return True if the last known validity id is equal to the current one
         */
        [[nodiscard]]
        bool isValid(id indetifier, id validity_id) const
        {
            return validity_id == m_metadata[m_indexes[indetifier]].validity_id;
        }

        /// Begin iterator of the data vector
        typename std::vector<TObjectType>::iterator begin() noexcept
        {
            return m_data.begin();
        }

        /// End iterator of the data vector
        typename std::vector<TObjectType>::iterator end() noexcept
        {
            return m_data.end();
        }

        /// Const begin iterator of the data vector
        typename std::vector<TObjectType>::const_iterator begin() const noexcept
        {
            return m_data.begin();
        }

        /// Const end iterator of the data vector
        typename std::vector<TObjectType>::const_iterator end() const noexcept
        {
            return m_data.end();
        }

        /** Removes all objects that match the provided predicate
         *
         * @tparam TCallback The callback's type, any callable should be fine
         * @param callback The predicate used to check an object has to be removed
         */
        template<typename TCallback>
        void remove_if(TCallback&& predicate)
        {
            for (uint32_t i{0}; i < m_data.size();) {
                if (predicate(m_data[i])) {
                    eraseViaData(i);
                } else {
                    ++i;
                }
            }
        }

        /** Pre allocates @p size slots in the vector
         *
         * @param size The number of slots to allocate in the vector
         */
        void reserve(size_t size)
        {
            m_data.reserve(size);
            m_metadata.reserve(size);
            m_indexes.reserve(size);
        }

        /// Return the validity id associated with the provided id
        [[nodiscard]]
        id getValidityID(id indetifier) const
        {
            return m_metadata[m_indexes[indetifier]].validity_id;
        }

        /// Returns a raw pointer to the first element of the data vector
        TObjectType* data()
        {
            return m_data.data();
        }

        /// Returns a reference to the data vector
        std::vector<TObjectType>& getData()
        {
            return m_data;
        }

        /// Returns a constant reference to the data vector
        const std::vector<TObjectType>& getData() const
        {
            return m_data;
        }

        /// Returns the id that would be used if an object was added
        [[nodiscard]]
        id getNextID() const
        {
            // This means that we have available slots
            if (m_metadata.size() > m_data.size()) {
                return m_metadata[m_data.size()].rid;
            }
            return m_data.size();
        }

        /// Erase all objects and invalidates all slots
        void clear()
        {
            // Remove all data
            m_data.clear();

            for (auto& m : m_metadata) {
                // Invalidate all slots
                ++m.validity_id;
            }
        }

        [[nodiscard]]
        bool isValidID(siv::id indetifier) const
        {
            return indetifier < m_indexes.size();
        }

    private:
        /** Creates a new slot in the vector
         *
         * @note If a slot is available it will be reused, if not a new one will be created.
         *
         * @return The id of the newly created slot.
         */
        id getFreeSlot()
        {
            const id indetifier = getFreeID();
            m_indexes[indetifier] = m_data.size();
            return indetifier;
        }

        /** Gets a id to a free slot.
         *
         * @note If an id is available it will be reused, if not a new one will be created.
         *
         * @return An id of a free slot.
         */
        id getFreeID()
        {
            // This means that we have available slots
            if (m_metadata.size() > m_data.size()) {
                // Update the validity id
                ++m_metadata[m_data.size()].validity_id;
                return m_metadata[m_data.size()].rid;
            }
            // A new slot has to be created
            const id new_id = m_data.size();
            m_metadata.push_back({new_id, 0});
            m_indexes.push_back(new_id);
            return new_id;
        }

    private:
        /// The struct holding additional information about an object
        struct Metadata
        {
            /// The reverse id, allowing to retrieve the id of the object from the data vector.
            id rid         = 0;
            /// An identifier that is changed when the object is erased, used to ensure a handle is still valid.
            id validity_id = 0;
        };

        /// The vector holding the actual objects.
        std::vector<TObjectType> m_data;
        /// The vector holding the associated metadata. It is accessed using the same index as for the data vector.
        std::vector<Metadata>    m_metadata;
        /// The vector that stores the data index for each id.
        std::vector<id>          m_indexes;
    };
}