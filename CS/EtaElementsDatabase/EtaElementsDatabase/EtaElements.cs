using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Diagnostics.Eventing.Reader;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using System.Xml.Linq;
using EtaElementsDatabase.Annotations;

namespace EtaElementsDatabase
{
    //****************************************************************************************************************************************************
    public interface INotifyItemPropertyChanged
    {
        /// <summary>Occurs when a property value of an item changes.</summary>
        event PropertyChangedEventHandler ItemPropertyChanged;
    }
    //****************************************************************************************************************************************************
    public class ObservableList<T> : IList<T>, INotifyPropertyChanged, INotifyItemPropertyChanged, INotifyCollectionChanged
    {
        private const string __CANNOT_INSERT_AN_ITEM_AT_AN_ARBITRARY_INDEX = "Cannot insert an item at an arbitrary index into a ObservableSortedList.";

        protected readonly object Mutext = new object();
        protected readonly List<T> InnerList;
        protected IComparer<T> InnerComparer;

        /// <summary>Gets the number of items stored in this collection.</summary>
        public int Count { get { lock (Mutext) { return InnerList.Count; } } }
        /// <summary>Returns false.</summary>
        public bool IsReadOnly { get { return false; } }

        /// <summary>Constructor.</summary>
        public ObservableList() : this(null, 4, null) { }
        /// <summary>Constructor.</summary>
        public ObservableList([NotNull] IComparer<T> comparer)
            : this(null, 4, comparer) {
            if (comparer == null) throw new ArgumentNullException("comparer");
        }
        /// <summary>Constructor.</summary>
        public ObservableList(int capacity) : this(null, capacity, null) { }
        /// <summary>Constructor.</summary>
        public ObservableList(int capacity, [NotNull] IComparer<T> comparer)
            : this(null, capacity, comparer) {
            if (comparer == null) throw new ArgumentNullException("comparer");
        }
        /// <summary>Constructor.</summary>
        public ObservableList([NotNull] IEnumerable<T> items)
            : this(items, 0, null) {
            if (items == null) throw new ArgumentNullException("items");
        }
        /// <summary>Constructor.</summary>
        public ObservableList([NotNull] IEnumerable<T> items, [NotNull] IComparer<T> comparer)
            : this(items, 0, comparer) {
            if (items == null) throw new ArgumentNullException("items");
            if (comparer == null) throw new ArgumentNullException("comparer");
        }
        /// <summary>Constructor.</summary>
        private ObservableList(IEnumerable<T> items, int capacity, IComparer<T> comparer) {
            InnerComparer = comparer ?? Comparer<T>.Default;
            if (items != null) {
                InnerList = new List<T>(items);
                foreach (var _item in InnerList) ItemAttach(_item);
            }
            else InnerList = new List<T>(capacity);
        }

        protected virtual void ItemAttach([NotNull] T item) { INotifyPropertyChanged _item_notify_property_changed = item as INotifyPropertyChanged; if (_item_notify_property_changed != null) _item_notify_property_changed.PropertyChanged += OnItemPropertyChanged; }
        protected virtual void ItemDeattach([NotNull] T item) { INotifyPropertyChanged _item_notify_property_changed = item as INotifyPropertyChanged; if (_item_notify_property_changed != null) _item_notify_property_changed.PropertyChanged -= OnItemPropertyChanged; }
        protected virtual void InnerClear() {
            foreach (var _item in InnerList) ItemDeattach(_item);
            InnerList.Clear();
            OnCollectionChanged_Reset(); OnPropertyChanged(nameof(Count));
        }
        protected virtual void InnerAdd([NotNull] T item) {
            InnerList.Add(item); ItemAttach(item);
            OnCollectionChanged_Added(item, InnerList.Count - 1); OnPropertyChanged(nameof(Count));
        }
        protected virtual void InnerInsert(int index, [NotNull] T item) {
            InnerList.Insert(index, item); ItemAttach(item);
            OnCollectionChanged_Added(item, index); OnPropertyChanged(nameof(Count));
        }
        protected virtual int InnerIndexOf([NotNull] T item) { return InnerList.IndexOf(item); }
        protected virtual bool InnerRemove([NotNull] T item) {
            int _i = InnerIndexOf(item); if (_i < 0) return false;
            InnerRemoveAt(_i);
            return true;
        }
        protected virtual void InnerRemoveAt(int index) {
            T _item = InnerList[index];
            InnerList.RemoveAt(index); ItemDeattach(_item);
            OnCollectionChanged_Removed(_item, index); OnPropertyChanged(nameof(Count));
        }
        protected virtual bool InnerContains([NotNull] T item) { return InnerIndexOf(item) >= 0; }
        protected virtual void InnerSort() { InnerList.Sort(); OnCollectionChanged_Reset(); }
        protected virtual void InnerChangeComparerAndResort([NotNull] IComparer<T> comparer) { InnerComparer = comparer; InnerList.Sort(); OnCollectionChanged_Reset(); }
        protected virtual T Inner_this_get(int index) { return InnerList[index]; }
        protected virtual void Inner_this_set(int index, [NotNull] T item) {
            T _item_prev = InnerList[index];
            ItemDeattach(_item_prev); ItemAttach(item);
            InnerList[index] = item;
        }

        /// <summary>Removes all items from this collection.</summary>
        public void Clear() { lock (Mutext) { InnerClear(); } }
        /// <summary>Adds an item to this collection, ensuring that it ends up at the correct place according to the sort order.</summary>
        public void Add([NotNull] T item) {
            if (item == null) throw new ArgumentNullException("item");
            lock (Mutext) { InnerAdd(item); }
        }
        /// <summary>Not supported on a sorted collection.</summary>
        public void Insert(int index, [NotNull] T item) {
            if (item == null) throw new ArgumentNullException("item");
            lock (Mutext) { InnerInsert(index, item); }
        }
        /// <summary>Removes the specified item, returning true if found or false otherwise.</summary>
        public bool Remove([NotNull] T item) {
            if (item == null) throw new ArgumentNullException("item");
            lock (Mutext) { return InnerRemove(item); }
        }
        /// <summary>Removes the specified item.</summary>
        public void RemoveAt(int index) { lock (Mutext) { InnerRemoveAt(index); } }
        /// <summary>Gets the index of the specified item, or -1 if not found. Only reference equality matches are considered.</summary>
        public int IndexOf([NotNull] T item) {
            if (item == null) throw new ArgumentNullException("item");
            lock (Mutext) { return InnerIndexOf(item); }
        }
        /// <summary>Returns a value indicating whether the specified item is contained in this collection.</summary>
        public bool Contains([NotNull] T item) {
            if (item == null) throw new ArgumentNullException("item");
            lock (Mutext) { return InnerContains(item); }
        }
        /// <summary>Copies all items to the specified array.</summary>
        public void CopyTo(T[] array, int array_index) { lock (Mutext) { InnerList.CopyTo(array, array_index); } }
        /// <summary>Sorts collection using internal comparer.</summary>
        public void Sort() { lock (Mutext) { InnerSort(); } }
        /// <summary>Changes comparer and resorts collection.</summary>
        public void ChangeComparerAndResort([NotNull] IComparer<T> comparer) {
            if (comparer == null) throw new ArgumentNullException("comparer");
            lock (Mutext) { InnerChangeComparerAndResort(comparer); }
        }
        /// <summary>Gets the item at the specified index. Does not support setting.</summary>
        public T this[int index] {
            get { lock (Mutext) { return Inner_this_get(index); } }
            set {
                lock (Mutext) {
                    if (value == null) throw new ArgumentNullException("value");
                    Inner_this_set(index, value);
                }
            }
        }

        /// <summary>Enumerates all items in sorted order.</summary>
        public IEnumerator<T> GetEnumerator() { lock (Mutext) { return InnerList.GetEnumerator(); } }
        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() { lock (Mutext) { return InnerList.GetEnumerator(); } }

        /// <summary>Triggered whenever the <see cref="Count" /> property changes as a result of adding/removing items.</summary>
        public event PropertyChangedEventHandler PropertyChanged;
        /// <summary>Occurs when a property value of an item changes.</summary>
        public event PropertyChangedEventHandler ItemPropertyChanged;
        /// <summary>Triggered whenever items are added/removed, and also whenever they are reordered due to item property changes.</summary>
        public event NotifyCollectionChangedEventHandler CollectionChanged;

        protected void OnPropertyChanged(string name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(name)); }
        protected void OnCollectionChanged_Reset() { NotifyCollectionChangedEventHandler _handler = CollectionChanged; if (_handler != null) _handler(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset)); }
        protected void OnCollectionChanged_Added(T item, int index) { NotifyCollectionChangedEventHandler _handler = CollectionChanged; if (_handler != null) _handler(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, item, index)); }
        protected void OnCollectionChanged_Removed(T item, int index) { NotifyCollectionChangedEventHandler _handler = CollectionChanged; if (_handler != null) _handler(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, item, index)); }
        protected void OnCollectionChanged_Moved(T item, int old_index, int new_index) { NotifyCollectionChangedEventHandler _handler = CollectionChanged; if (_handler != null) _handler(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, item, new_index, old_index)); }

        protected virtual void OnItemPropertyChanged(object sender, PropertyChangedEventArgs e) { PropertyChangedEventHandler _handler = ItemPropertyChanged; if (_handler != null) _handler(this, e); }
    }
    //****************************************************************************************************************************************************
    public class ObservableSortedList<T> : ObservableList<T>
    {
        private const string __CANNOT_INSERT_AN_ITEM_AT_AN_ARBITRARY_INDEX = "Cannot insert an item at an arbitrary index into a ObservableSortedList.";

        /// <summary>Constructor.</summary>
        /// <remarks>Certain serialization libraries require a parameterless constructor.</remarks>
        public ObservableSortedList() : base() { }
        /// <summary>Constructor.</summary>
        public ObservableSortedList([NotNull] IComparer<T> comparer) : base(comparer) { }
        /// <summary>Constructor.</summary>
        public ObservableSortedList(int capacity) : base(capacity) { }
        /// <summary>Constructor.</summary>
        public ObservableSortedList(int capacity, [NotNull] IComparer<T> comparer) : base(capacity, comparer) { }
        /// <summary>Constructor.</summary>
        public ObservableSortedList([NotNull] IEnumerable<T> items) : base(items) { InnerList.Sort(InnerComparer); }
        /// <summary>Constructor.</summary>
        public ObservableSortedList([NotNull] IEnumerable<T> items, [NotNull] IComparer<T> comparer) : base(items, comparer) { InnerList.Sort(InnerComparer); }

        protected override void InnerAdd([NotNull] T item) {
            int i = InnerList.BinarySearch(item, InnerComparer);
            if (i < 0) i = ~i;
            else do i++; while (i < InnerList.Count && InnerComparer.Compare(InnerList[i], item) == 0);

            InnerList.Insert(i, item); ItemAttach(item);
            OnCollectionChanged_Added(item, i); OnPropertyChanged(nameof(Count));
        }
        protected override void InnerInsert(int index, [NotNull] T item) { throw new InvalidOperationException(__CANNOT_INSERT_AN_ITEM_AT_AN_ARBITRARY_INDEX); }
        protected override int InnerIndexOf([NotNull] T item) {
            // Binary search is used to make the operation more efficient.
            int _i = InnerList.BinarySearch(item, InnerComparer);
            if (_i < 0) return -1;
            if (object.ReferenceEquals(InnerList[_i], item)) return _i;
            // Search downwards
            for (int _s = _i - 1; _s >= 0 && InnerComparer.Compare(InnerList[_s], item) == 0; _s--)
                if (object.ReferenceEquals(InnerList[_s], item))
                    return _s;
            // Search upwards
            for (int _s = _i + 1; _s < InnerList.Count && InnerComparer.Compare(InnerList[_s], item) == 0; _s++)
                if (object.ReferenceEquals(InnerList[_s], item))
                    return _s;
            // Not found
            return -1;
        }
        protected override void InnerSort() { }
        protected override void Inner_this_set(int index, [NotNull] T item) { throw new InvalidOperationException(__CANNOT_INSERT_AN_ITEM_AT_AN_ARBITRARY_INDEX); }

        protected override void OnItemPropertyChanged(object sender, PropertyChangedEventArgs e) {
            base.OnItemPropertyChanged(sender, e);

            T _item = (T)sender;
            int _old_index = InnerList.IndexOf(_item);

            // See if item should now be sorted to a different position
            if (Count <= 1 || (_old_index == 0 || InnerComparer.Compare(InnerList[_old_index - 1], _item) <= 0) && (_old_index == Count - 1 || InnerComparer.Compare(_item, InnerList[_old_index + 1]) <= 0)) return;

            // Find where it should be inserted 
            InnerList.RemoveAt(_old_index);
            int _new_index = InnerList.BinarySearch(_item, InnerComparer);
            if (_new_index < 0) _new_index = ~_new_index; else do _new_index++; while (_new_index < InnerList.Count && InnerComparer.Compare(InnerList[_new_index], _item) == 0);
            InnerList.Insert(_new_index, _item);
            OnCollectionChanged_Moved(_item, _old_index, _new_index);
        }
    }
    //****************************************************************************************************************************************************
    public class ReadOnlyObservableList<T> : IList<T>, INotifyPropertyChanged, INotifyItemPropertyChanged, INotifyCollectionChanged
    {
        private const string __CANNOT_MODIFY_READONLY = "Cannot modify readonly ReadOnlyObservableSortedList.";

        private readonly ObservableList<T> _observable_list;

        /// <summary>Gets the number of items stored in this collection.</summary>
        public int Count { get { return _observable_list.Count; } }
        /// <summary>Returns false.</summary>
        public bool IsReadOnly { get { return true; } }

        /// <summary>Constructor.</summary>
        public ReadOnlyObservableList([NotNull] ObservableList<T> observable_list) {
            if (observable_list == null) throw new ArgumentNullException("observable_list");
            _observable_list = observable_list;
            _observable_list.PropertyChanged += property_changed; _observable_list.CollectionChanged += collection_changed; _observable_list.ItemPropertyChanged += item_property_changed;
        }

        /// <summary>Removes all items from this collection.</summary>
        public void Clear() { throw new InvalidOperationException(__CANNOT_MODIFY_READONLY); }
        /// <summary>Adds an item to this collection, ensuring that it ends up at the correct place according to the sort order.</summary>
        public void Add([NotNull] T item) { throw new InvalidOperationException(__CANNOT_MODIFY_READONLY); }
        /// <summary>Not supported on a sorted collection.</summary>
        public void Insert(int index, [NotNull] T item) { throw new InvalidOperationException(__CANNOT_MODIFY_READONLY); }
        /// <summary>Removes the specified item, returning true if found or false otherwise.</summary>
        public bool Remove([NotNull] T item) { throw new InvalidOperationException(__CANNOT_MODIFY_READONLY); }
        /// <summary>Removes the specified item.</summary>
        public void RemoveAt(int index) { throw new InvalidOperationException(__CANNOT_MODIFY_READONLY); }
        /// <summary>Gets the index of the specified item, or -1 if not found. Only reference equality matches are considered.</summary>
        public int IndexOf([NotNull] T item) { return _observable_list.IndexOf(item); }
        /// <summary>Returns a value indicating whether the specified item is contained in this collection.</summary>
        public bool Contains([NotNull] T item) { return _observable_list.Contains(item); }
        /// <summary>Copies all items to the specified array.</summary>
        public void CopyTo(T[] array, int array_index) { _observable_list.CopyTo(array, array_index); }
        /// <summary>Gets the item at the specified index. Does not support setting.</summary>
        public T this[int index] { get { return _observable_list[index]; } set { throw new InvalidOperationException(__CANNOT_MODIFY_READONLY); } }

        /// <summary>Enumerates all items in sorted order.</summary>
        public IEnumerator<T> GetEnumerator() { return _observable_list.GetEnumerator(); }
        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() { return _observable_list.GetEnumerator(); }

        /// <summary>Triggered whenever the <see cref="Count" /> property changes as a result of adding/removing items.</summary>
        public event PropertyChangedEventHandler PropertyChanged;
        /// <summary>Occurs when a property value of an item changes.</summary>
        public event PropertyChangedEventHandler ItemPropertyChanged;
        /// <summary>Triggered whenever items are added/removed, and also whenever they are reordered due to item property changes.</summary>
        public event NotifyCollectionChangedEventHandler CollectionChanged;

        private void property_changed(object sender, PropertyChangedEventArgs args) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, args); }
        private void item_property_changed(object sender, PropertyChangedEventArgs args) { PropertyChangedEventHandler _handler = ItemPropertyChanged; if (_handler != null) _handler(this, args); }
        private void collection_changed(object sender, NotifyCollectionChangedEventArgs args) { NotifyCollectionChangedEventHandler _handler = CollectionChanged; if (_handler != null) _handler(this, args); }
    }
    //****************************************************************************************************************************************************

    //****************************************************************************************************************************************************
    public abstract class CElement : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        internal static readonly HashSet<string> HASH_SET_PLACEMENT_BOXES = new HashSet<string>();
        internal static readonly ObservableSortedList<string> LIST_PLACEMENT_BOXES = new ObservableSortedList<string>();
        internal static readonly ReadOnlyObservableList<string> ROLIST_PLACEMENT_BOXES = new ReadOnlyObservableList<string>(LIST_PLACEMENT_BOXES);

        internal CElementContainer _element_parent;
        internal readonly Dictionary<string, CElementItem.CElementEntity.CName> _dict_entities = new Dictionary<string, CElementItem.CElementEntity.CName>();
        internal readonly ObservableSortedList<CElementItem.CElementEntity> _list_entities_distinct = new ObservableSortedList<CElementItem.CElementEntity>();
        internal readonly ObservableSortedList<CElementItem.CElementEntity.CName> _list_entities_by_name = new ObservableSortedList<CElementItem.CElementEntity.CName>();
        private readonly ReadOnlyObservableList<CElementItem.CElementEntity> _rolist_entities_distinct;
        private readonly ReadOnlyObservableList<CElementItem.CElementEntity.CName> _rolist_entities_by_name;

        internal readonly string _name;

        public static ReadOnlyObservableList<string> PlacementBoxes { get { return ROLIST_PLACEMENT_BOXES; } }

        public CElementContainer ElementRoot { get { return _element_parent != null ? _element_parent.ElementRoot : this as CElementContainer; } }
        public CElementContainer ElementParent { get { return _element_parent; } }
        public string Name { get { return _name; } }
        public string Path { get { return _element_parent != null ? string.Format("{0}.{1}", _element_parent.Path, _name) : _name; } }
        public string FriendlyPath { get { return _element_parent != null && !(_element_parent is CEtaElementsDatabase) ? string.Format("{0}\\{1}", _element_parent.FriendlyPath, _name) : _name; } }
        public ReadOnlyObservableList<CElementItem.CElementEntity> EntitiesDistinct { get { return _rolist_entities_distinct; } }
        public ReadOnlyObservableList<CElementItem.CElementEntity.CName> EntitiesByName { get { return _rolist_entities_by_name; } }

        protected CElement(string name) {
            if (name == null) throw new ArgumentNullException("name");
            _name = name;
            _rolist_entities_distinct = new ReadOnlyObservableList<CElementItem.CElementEntity>(_list_entities_distinct);
            _rolist_entities_by_name = new ReadOnlyObservableList<CElementItem.CElementEntity.CName>(_list_entities_by_name);
        }

        protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(propery_name)); }

        public CElementItem.CElementEntity.CName NameFind(string name) {
            if (name == null) throw new ArgumentNullException("name");
            name = name.ToUpper();
            CElementItem.CElementEntity.CName _element_entity_name; _dict_entities.TryGetValue(name, out _element_entity_name);
            return _element_entity_name;
        }
        public CElementItem.CElementEntity.CName NameFindGlobal(string name) {
            if (name == null) throw new ArgumentNullException("name");
            CElementContainer _element_root = ElementRoot;
            return _element_root != null ? _element_root.NameFind(name) : null;
        }

        internal void ElementEntityAdd(CElementItem.CElementEntity entity) {
            if (_element_parent != null) _element_parent.ElementEntityAdd(entity);
            _list_entities_distinct.Add(entity); OnPropertyChanged(nameof(EntitiesDistinct));
        }
        internal void ElementEntityRemove(CElementItem.CElementEntity entity) {
            if (_element_parent != null) _element_parent.ElementEntityRemove(entity);
            _list_entities_distinct.Remove(entity); OnPropertyChanged(nameof(EntitiesDistinct));
        }

        internal void NameAdd(CElementItem.CElementEntity.CName entity_name) {
            if (_dict_entities.ContainsKey(entity_name.Name)) throw new ArgumentException("Already exists", "name");
            if (_element_parent != null) _element_parent.NameAdd(entity_name);
            _dict_entities[entity_name.Name] = entity_name; _list_entities_by_name.Add(entity_name); OnPropertyChanged(nameof(EntitiesByName));
        }
        internal void NameRemove(string name) {
            CElementItem.CElementEntity.CName _element_entity_name;
            if (!_dict_entities.TryGetValue(name, out _element_entity_name)) throw new ArgumentException("Name not found", "old_name");
            if (_element_parent != null) _element_parent.NameRemove(name);
            _dict_entities.Remove(name); _list_entities_by_name.Remove(_element_entity_name); OnPropertyChanged(nameof(EntitiesByName));
        }
        internal void NameRename(string old_name, string new_name) {
            CElementItem.CElementEntity.CName _element_entity_name;
            if (!_dict_entities.TryGetValue(old_name, out _element_entity_name)) throw new ArgumentException("Name not found", "old_name");
            if (_dict_entities.ContainsKey(new_name)) throw new ArgumentException("Name already exists", "new_name");
            if (_element_parent != null) _element_parent.NameRename(old_name, new_name);
            _dict_entities.Remove(old_name); _dict_entities[new_name] = _element_entity_name; OnPropertyChanged(nameof(EntitiesByName));
        }

        internal static void RegisterPlacementBox(string placement_box) { if (HASH_SET_PLACEMENT_BOXES.Add(placement_box)) LIST_PLACEMENT_BOXES.Add(placement_box); }

        public IEnumerable<CElementItem.CElementEntity.CName> IncrementalSearch(string search_string) {
            search_string = search_string.ToUpper();
            return _dict_entities.Where(kvp_entity => kvp_entity.Key.Contains(search_string)).Select(kvp_entity => kvp_entity.Value);
        }

        public abstract void XMLParse(XElement xml_element);
        public abstract XElement XMLGenerate();
    }
    //****************************************************************************************************************************************************
    public class CElementContainer : CElement
    {
        internal readonly CElement[] _child_elements;

        public CElement[] ChildElements { get { return _child_elements; } }

        public CElementContainer(string name, params CElement[] child_elements) : base(name) { _child_elements = child_elements; foreach (CElement _child_element in _child_elements) { _child_element._element_parent = this; } }

        protected void XMLParseChilds(XElement xml_element_container) {
            Dictionary<string, XElement> _dict_xml_element_containers = xml_element_container.Elements("ElementContainer").Select(element => { XAttribute _attribute_name = element.Attribute("Name"); if (_attribute_name == null) throw new ArgumentException("No Name attribute"); return new KeyValuePair<XAttribute, XElement>(_attribute_name, element); }).ToDictionary(kvp => kvp.Key.Value, kvp => kvp.Value);
            Dictionary<string, XElement> _dict_xml_element_items = xml_element_container.Elements("ElementItem").Select(element => { XAttribute _attribute_name = element.Attribute("Name"); if (_attribute_name == null) throw new ArgumentException("No Name attribute"); return new KeyValuePair<XAttribute, XElement>(_attribute_name, element); }).ToDictionary(kvp => kvp.Key.Value, kvp => kvp.Value);

            foreach (CElement _child_element in _child_elements) {
                CElementContainer _child_element_container = _child_element as CElementContainer;
                CElementItem _child_element_item = _child_element as CElementItem;
                XElement _xml_element_child;
                if (_child_element_container != null) { if (_dict_xml_element_containers.TryGetValue(_child_element_container._name, out _xml_element_child)) { _child_element_container.XMLParse(_xml_element_child); } }
                else if (_child_element_item != null) { if (_dict_xml_element_items.TryGetValue(_child_element_item._name, out _xml_element_child)) { _child_element_item.XMLParse(_xml_element_child); } }
            }
        }
        protected void XMLGenerateChilds(XElement xml_element_container) {
            foreach (CElement _child_element in _child_elements) {
                xml_element_container.Add(_child_element.XMLGenerate());
            }
        }

        public override void XMLParse(XElement xml_element_container) {
            if (xml_element_container == null) throw new ArgumentNullException("xml_element_container");
            XMLParseChilds(xml_element_container);
        }
        public override XElement XMLGenerate() {
            XElement _xml_element_container = new XElement("ElementContainer"); _xml_element_container.Add(new XAttribute("Name", _name));
            XMLGenerateChilds(_xml_element_container);
            return _xml_element_container;
        }
    }
    //****************************************************************************************************************************************************
    public abstract class CElementItem : CElement
    {
        internal readonly CParameter[] _parameters;

        public CParameter[] Parameters { get { return _parameters; } }

        protected CElementItem(string name, params CParameter[] parameters) : base(name) { _parameters = parameters; for (uint _i = 0; _i < _parameters.Length; _i++) { _parameters[_i]._index = _i; } }

        public CElementEntity ElementEntityAdd() {
            CElementEntity _element_entity = EntityFactory(); ElementEntityAdd(_element_entity);
            return _element_entity;
        }

        public override void XMLParse(XElement xml_element_item) {
            if (xml_element_item == null) throw new ArgumentNullException("xml_element");
            foreach (XElement _xml_entity in xml_element_item.Elements("Entity")) {
                CElementEntity _element_entity = ElementEntityAdd();

                XAttribute _attribute_name = _xml_entity.Attribute("Name");
                if (_attribute_name == null) throw new ArgumentException("No Name attribute");
                foreach (string _entity_name in _attribute_name.Value.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries)) { _element_entity.NameAdd(_entity_name); }

                foreach (XElement _xml_parameter in _xml_entity.Elements("Parameter")) {
                    XAttribute _attribute_parameter_name = _xml_parameter.Attribute("Name");
                    if (_attribute_parameter_name == null) throw new ArgumentException("No Name attribute");
                    _element_entity.ParameterGet(_attribute_parameter_name.Value, true, _xml_parameter.Value);
                }

                foreach (XElement _xml_package in _xml_entity.Elements("Package")) {
                    XAttribute _attribute_package_name = _xml_package.Attribute("Name");
                    XAttribute _attribute_count = _xml_package.Attribute("Count");
                    if (_attribute_package_name == null) throw new ArgumentException("No Name attribute");
                    if (_attribute_count == null) throw new ArgumentException("No Count attribute");
                    _element_entity.PackageGet(_attribute_package_name.Value, true).Count = uint.Parse(_attribute_count.Value);
                }

                XElement _xml_placement = _xml_entity.Element("Placement");
                if (_xml_placement != null) {
                    XAttribute _attribute_box = _xml_placement.Attribute("Box");
                    XAttribute _attribute_row = _xml_placement.Attribute("Row");
                    XAttribute _attribute_column = _xml_placement.Attribute("Column");
                    if (_attribute_box == null) throw new ArgumentException("No Box attribute");
                    if (_attribute_row == null) throw new ArgumentException("No Row attribute");
                    if (_attribute_column == null) throw new ArgumentException("No Column attribute");
                    _element_entity._placement_box = _attribute_box.Value; RegisterPlacementBox(_element_entity._placement_box);
                    _element_entity._placement_row = uint.Parse(_attribute_row.Value);
                    _element_entity._placement_row = uint.Parse(_attribute_column.Value);
                }

                foreach (XElement _xml_datasheet in _xml_entity.Elements("Datasheet")) {
                    XAttribute _attribute_filename = _xml_datasheet.Attribute("Filename");
                    XAttribute _attribute_page = _xml_datasheet.Attribute("Page");
                    if (_attribute_filename == null) throw new ArgumentException("No Filename attribute");
                    if (_attribute_page == null) throw new ArgumentException("No Page attribute");
                    _element_entity.DatasheetAdd(_attribute_filename.Value, uint.Parse(_attribute_page.Value));
                }

                XElement _xml_comment = _xml_entity.Element("Comment");
                if (_xml_comment != null) { _element_entity.Comment = _xml_comment.Value; }
            }
        }
        public override XElement XMLGenerate() {
            XElement _xml_element = new XElement("ElementItem"); _xml_element.Add(new XAttribute("Name", _name));
            foreach (CElementEntity _element_entity in EntitiesDistinct) {
                XElement _xml_entity = new XElement("Entity"); _xml_element.Add(_xml_entity);
                _xml_entity.Add(new XAttribute("Name", _element_entity.NameTotal));
                foreach (CParameter.CParameterEntity _parameter_entity in _element_entity.ParameterEntities) {
                    XElement _xml_parameter = new XElement("Parameter", _parameter_entity.StringValue);
                    _xml_parameter.Add(new XAttribute("Name", _parameter_entity.Parameter.ParameterName));
                    _xml_entity.Add(_xml_parameter);
                }
                foreach (CPackage.CPackageEntity _package_entity in _element_entity.PackageEntities) {
                    _xml_entity.Add(new XElement("Package", new XAttribute("Name", _package_entity.Package.PackageName), new XAttribute("Count", _package_entity.Count.ToString())));
                }
                _xml_entity.Add(new XElement("Placement", new XAttribute("Box", _element_entity._placement_box), new XAttribute("Row", _element_entity._placement_row.ToString()), new XAttribute("Column", _element_entity._placement_column.ToString())));
                foreach (CElementEntity.CDatasheet _datasheet in _element_entity.Datasheets) {
                    _xml_entity.Add(new XElement("Datasheet", new XAttribute("Filename", _datasheet.Filename), new XAttribute("Page", _datasheet.Page.ToString())));
                }
                if (!string.IsNullOrWhiteSpace(_element_entity.Comment)) _xml_entity.Add(new XElement("Comment", _element_entity.Comment));
            }
            return _xml_element;
        }

        protected abstract CElementEntity EntityFactory();

        //    **************
        public abstract class CParameter : IEquatable<CParameter>, IComparable<CParameter>, IComparable
        {
            private readonly string _parameter_name;
            private readonly string _binding_name;
            private readonly TextAlignment _text_alignment;

            internal uint _index;

            public string ParameterName { get { return _parameter_name; } }
            public string BindingName { get { return _binding_name; } }
            public TextAlignment TextAlignment { get { return _text_alignment; } }

            protected CParameter(string parameter_name, TextAlignment text_alignment) {
                if (parameter_name == null) throw new ArgumentNullException("parameter_name");
                _parameter_name = parameter_name;
                _binding_name = _parameter_name.Replace(" ", string.Empty);
                _text_alignment = text_alignment;
            }

            public abstract CParameterEntity FactoryEntity(CElementEntity element_entity_owner, string value);

            public bool Equals(CParameter other) { if (other == null) return false; return _index.Equals(other._index); }
            public int CompareTo(object obj) { return CompareTo(obj as CParameter); }
            public int CompareTo(CParameter other) { if (other == null) return 1; return _index.CompareTo(other._index); }

            //    **************
            public abstract class CParameterEntity : INotifyPropertyChanged, IEquatable<CParameterEntity>, IComparable<CParameterEntity>, IComparable
            {
                public event PropertyChangedEventHandler PropertyChanged;

                private CElementEntity _element_entity_owner;

                public CElementEntity ElementEntityOwner { get { return _element_entity_owner; } }
                public abstract CParameter Parameter { get; }
                public abstract object Value { get; set; }
                public string StringValue { get { return GenerateValue(Value); } set { Value = ParseValue(value); _element_entity_owner.OnPropertyChanged(Parameter.BindingName); } }

                protected CParameterEntity(CElementEntity element_entity_owner) { _element_entity_owner = element_entity_owner; }

                protected abstract object ParseValue(string value);
                protected abstract string GenerateValue(object value);

                protected internal void OnPropertyChanged(string propery_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(propery_name)); }

                public bool Equals(CParameterEntity other) { if (other == null) return false; return Parameter.Equals(other.Parameter); }
                public int CompareTo(object obj) { return CompareTo(obj as CParameterEntity); }
                public int CompareTo(CParameterEntity other) { if (other == null) return 1; return Parameter.CompareTo(other.Parameter); }
            }
            //    **************
        }
        //    **************
        public class CParameterString : CParameter
        {
            private readonly string _regular_expression;

            public string RegularExpression { get { return _regular_expression; } }

            public CParameterString(string parameter_name, string regular_expression) : base(parameter_name, TextAlignment.Left) { _regular_expression = regular_expression; }

            public override CParameterEntity FactoryEntity(CElementEntity element_entity_owner, string value) { return new CParameterEntityString(element_entity_owner, this, value); }

            //    **************
            public class CParameterEntityString : CParameterEntity
            {
                private readonly CParameterString _parameter;
                private string _value = string.Empty;

                public CParameterString ParameterString { get { return _parameter; } }
                public override CParameter Parameter { get { return _parameter; } }
                public override object Value { get { return _value; } set { if (value == null) throw new ArgumentNullException("value"); _value = _CheckValueThrow((string)value); OnPropertyChanged(nameof(Value)); OnPropertyChanged(nameof(StringValue)); } }

                internal CParameterEntityString(CElementEntity element_entity_owner, CParameterString parameter, string value) : base(element_entity_owner) { _parameter = parameter; if (value != null) _value = _CheckValueThrow((string)ParseValue(value)); }

                protected override object ParseValue(string value) { return _CheckValueThrow(value); }
                protected override string GenerateValue(object value) { return (string)value; }
                private string _CheckValueThrow(string value) {
                    value = value.Trim();
                    if (!_CheckValue(value)) throw new FormatException("Invalid value");
                    return value;
                }
                private bool _CheckValue(string value) {
                    value = value.Trim();
                    if (_parameter._regular_expression != null) return Regex.IsMatch(value, _parameter._regular_expression);
                    return true;
                }
            }
            //    **************
        }
        //    **************
        public class CParameterIntegral : CParameter
        {
            private readonly double? _min, _max;
            private readonly string _measurement_unit_name;
            private readonly bool _allow_unit_modifier;

            public double? Min { get { return _min; } }
            public double? Max { get { return _max; } }
            public string MeasurementUnitName { get { return _measurement_unit_name; } }
            public bool AllowUnitModifier { get { return _allow_unit_modifier; } }

            public CParameterIntegral(string parameter_name, double? min, double? max, string measurement_unit_name, bool allow_unit_modifier) : base(parameter_name, TextAlignment.Right) { _min = min; _max = max; _measurement_unit_name = measurement_unit_name; _allow_unit_modifier = allow_unit_modifier; }

            public override CParameterEntity FactoryEntity(CElementEntity element_entity_owner, string value) { return new CParameterEntityIntegral(element_entity_owner, this, value); }

            //    **************
            public class CParameterEntityIntegral : CParameterEntity
            {
                private readonly CParameterIntegral _parameter;
                private double _value;

                public CParameterIntegral ParameterIntegral { get { return _parameter; } }
                public override CParameter Parameter { get { return _parameter; } }
                public override object Value { get { return _value; } set { _value = _CheckValueThrow((double)value); OnPropertyChanged(nameof(Value)); OnPropertyChanged(nameof(StringValue)); } }

                internal CParameterEntityIntegral(CElementEntity element_entity_owner, CParameterIntegral parameter, string value) : base(element_entity_owner) { _parameter = parameter; if (value != null) _value = _CheckValueThrow((double)ParseValue(value)); }

                protected override object ParseValue(string value) {
                    if (value == null) throw new ArgumentNullException("value");
                    value = value.Trim();
                    if (_parameter._measurement_unit_name != null && value.EndsWith(_parameter._measurement_unit_name, StringComparison.OrdinalIgnoreCase)) { value = value.Substring(0, value.Length - _parameter._measurement_unit_name.Length).Trim(); }

                    string[] _values_of_unit = { value }; double _unit_weight = 1;
                    if (_parameter._allow_unit_modifier) {
                        foreach (SUnit _unit_modifier in SUnit.__UNIT_MODIFIERS.Where(x => x.Multiplier == _CheckValue(x.Multiplier))) {
                            _values_of_unit = value.Split(_unit_modifier.Symbol);
                            if (_values_of_unit.Length > 2) throw new FormatException("Invalid format of Integral entity");
                            if (_values_of_unit.Length == 2) { _unit_weight = _unit_modifier.Multiplier; break; }
                        }
                    }

                    value = _values_of_unit.Length == 2 ? string.Format("{0}.{1}", _values_of_unit[0], _values_of_unit[1]) : _values_of_unit[0];
                    if (value.Length == 0) value = "0";
                    return double.Parse(value, NumberStyles.AllowLeadingSign | NumberStyles.AllowDecimalPoint, CultureInfo.InvariantCulture) * _unit_weight;
                }
                protected override string GenerateValue(object value) {
                    double _value_tmp = (double)value;
                    string _value_str;

                    int _unit_index = _parameter._allow_unit_modifier ? (_value_tmp == 0 ? 0 : (int)Math.Floor(Math.Log10(_value_tmp) / 3)) : 0;

                    if (_unit_index == 0) _value_str = _value_tmp.ToString("F16", CultureInfo.InvariantCulture);
                    else {
                        if (_unit_index > 8) _unit_index = 8;
                        else if (_unit_index < -8) _unit_index = -8;
                        SUnit _unit_modifier = SUnit.__UNIT_MODIFIERS.First(x => x.UnitIndex == _unit_index);
                        double _unit_value = _value_tmp / _unit_modifier.Multiplier;
                        _value_str = _unit_value.ToString("F16", CultureInfo.InvariantCulture).Replace('.', _unit_modifier.Symbol).TrimEnd('0');
                    }
                    if (_value_str.Contains(".")) _value_str = _value_str.TrimEnd('0'); _value_str = _value_str.TrimEnd('.');

                    if (_parameter._measurement_unit_name != null) _value_str = string.Format("{0} {1}", _value_str, _parameter._measurement_unit_name);

                    return _value_str;
                }
                private double _CheckValueThrow(double value) {
                    if (value != _CheckValue(value)) throw new FormatException("Value out of acceptable range");
                    return value;
                }
                private double _CheckValue(double value) {
                    if (_parameter._min.HasValue && value < _parameter._min.Value) return _parameter._min.Value;
                    if (_parameter._max.HasValue && value > _parameter._max.Value) return _parameter._max.Value;
                    return value;
                }

                //    **************
                private struct SUnit
                {
                    public static readonly SUnit[] __UNIT_MODIFIERS =
                    {
                        new SUnit(8, 'Y', 1000000000000000000000000.0),   // Yotta
                        new SUnit(7, 'Z', 1000000000000000000000.0),      // Zetta
                        new SUnit(6, 'E', 1000000000000000000.0),         // Exa
                        new SUnit(5, 'P', 1000000000000000.0),            // Peta
                        new SUnit(4, 'T', 1000000000000.0),               // Tera
                        new SUnit(3, 'G', 1000000000.0),                  // Giga
                        new SUnit(2, 'M', 1000000.0),                     // Mega
                        new SUnit(1, 'K', 1000.0),                        // Kilo
                        new SUnit(-1, 'm', 0.001),                        // milli
                        new SUnit(-2, 'u', 0.000001),                     // micro
                        new SUnit(-3, 'n', 0.000000001),                  // nano
                        new SUnit(-4, 'p', 0.000000000001),               // pico
                        new SUnit(-5, 'f', 0.000000000000001),            // femto
                        new SUnit(-6, 'a', 0.000000000000000001),         // atto
                        new SUnit(-7, 'z', 0.000000000000000000001),      // zepto
                        new SUnit(-8, 'y', 0.000000000000000000000001),   // yocto
                    };

                    public readonly int UnitIndex;
                    public readonly char Symbol;
                    public readonly double Multiplier;

                    public SUnit(int unit_index, char symbol, double multiplier) { UnitIndex = unit_index; Symbol = symbol; Multiplier = multiplier; }
                }
                //    **************
            }
            //    **************
        }
        //    **************
        public class CParameterEnum : CParameter
        {
            private readonly string _regular_expression_for_other;
            private readonly string[] _values;

            public string RegularExpressionForOther { get { return _regular_expression_for_other; } }
            public string[] Values { get { return _values; } }

            public CParameterEnum(string parameter_name, string regular_expression_for_other, params string[] values) : base(parameter_name, TextAlignment.Center) { _regular_expression_for_other = regular_expression_for_other; _values = values; if (_values.Length == 0) throw new ArgumentException("No enum values", "values"); }

            public override CParameterEntity FactoryEntity(CElementEntity element_entity_owner, string value) { return new CParameterEntityEnum(element_entity_owner, this, value); }

            //    **************
            public class CParameterEntityEnum : CParameterEntity
            {
                private readonly CParameterEnum _parameter;
                private string _value;

                public CParameterEnum ParameterEnum { get { return _parameter; } }
                public override CParameter Parameter { get { return _parameter; } }
                public override object Value { get { return _value; } set { if (value == null) throw new ArgumentNullException("value"); _value = _CheckValueThrow((string)value); OnPropertyChanged(nameof(Value)); OnPropertyChanged(nameof(StringValue)); } }

                internal CParameterEntityEnum(CElementEntity element_entity_owner, CParameterEnum parameter, string value) : base(element_entity_owner) { _parameter = parameter; _value = value != null ? _CheckValueThrow((string)ParseValue(value)) : _parameter._values[0]; }

                protected override object ParseValue(string value) {
                    if (value == null) throw new ArgumentNullException("value");
                    value = value.Trim();
                    try { return _parameter._values.First(x => x.Equals(value, StringComparison.OrdinalIgnoreCase)); }
                    catch (InvalidOperationException) {
                        if (_parameter._regular_expression_for_other != null && Regex.IsMatch(value, _parameter._regular_expression_for_other)) return value;
                        throw new FormatException("Invalid enum value");
                    }
                }
                protected override string GenerateValue(object value) { return (string)value; }
                private string _CheckValueThrow(string value) {
                    value = value.Trim();
                    if (!_CheckValue(value)) throw new FormatException("Invalid value");
                    return value;
                }
                private bool _CheckValue(string value) {
                    value = value.Trim();
                    return _parameter._values.Any(x => x.Equals(value, StringComparison.OrdinalIgnoreCase)) || (_parameter._regular_expression_for_other != null && Regex.IsMatch(value, _parameter._regular_expression_for_other));
                }
            }
            //    **************
        }
        //    **************
        public abstract class CElementEntity : IEquatable<CElementEntity>, IComparable<CElementEntity>, IComparable, INotifyPropertyChanged
        {
            public event PropertyChangedEventHandler PropertyChanged;

            internal readonly Guid _guid = Guid.NewGuid();
            internal readonly Dictionary<string, CParameter.CParameterEntity> _dict_parameter_entities = new Dictionary<string, CParameter.CParameterEntity>();
            internal readonly Dictionary<string, CPackage.CPackageEntity> _dict_package_entities = new Dictionary<string, CPackage.CPackageEntity>();
            internal readonly ObservableSortedList<CName> _list_names = new ObservableSortedList<CName>();
            internal readonly ObservableSortedList<CParameter.CParameterEntity> _list_parameter_entities = new ObservableSortedList<CParameter.CParameterEntity>();
            internal readonly ObservableSortedList<CPackage.CPackageEntity> _list_package_entities = new ObservableSortedList<CPackage.CPackageEntity>();
            internal readonly ObservableList<CDatasheet> _list_datasheets = new ObservableSortedList<CDatasheet>();
            private readonly ReadOnlyObservableList<CName> _rolist_names;
            private readonly ReadOnlyObservableList<CParameter.CParameterEntity> _rolist_parameter_entities;
            private readonly ReadOnlyObservableList<CPackage.CPackageEntity> _rolist_package_entities;
            private readonly ReadOnlyObservableList<CDatasheet> _rolist_datasheets;
            private string _name_total = string.Empty;
            internal readonly CElementItem _element_item;
            internal string _comment;
            internal string _placement_box = string.Empty;
            internal uint _placement_row;
            internal uint _placement_column;

            public ReadOnlyObservableList<CName> Names { get { return _rolist_names; } }
            public ReadOnlyObservableList<CParameter.CParameterEntity> ParameterEntities { get { return _rolist_parameter_entities; } }
            public ReadOnlyObservableList<CPackage.CPackageEntity> PackageEntities { get { return _rolist_package_entities; } }
            public ReadOnlyObservableList<CDatasheet> Datasheets { get { return _rolist_datasheets; } }
            public string NameTotal { get { return _name_total; } }
            public CElementItem ElementItem { get { return _element_item; } }
            public string Comment { get { return _comment; } set { _comment = value; OnPropertyChanged(nameof(Comment)); } }
            public string PlacementBox { get { return _placement_box; } set { _placement_box = value ?? string.Empty; OnPropertyChanged(nameof(PlacementBox)); RegisterPlacementBox(_placement_box); } }
            public uint PlacementRow { get { return _placement_row; } set { _placement_row = value; OnPropertyChanged(nameof(PlacementRow)); } }
            public uint PlacementColumn { get { return _placement_column; } set { _placement_column = value; OnPropertyChanged(nameof(PlacementColumn)); } }

            protected CElementEntity([NotNull] CElementItem element_item) {
                if (element_item == null) throw new ArgumentNullException("element_item");
                _element_item = element_item;
                _list_names.CollectionChanged += (sender, args) => { OnPropertyChanged(nameof(Names)); MakeNameTotal(); };
                _list_parameter_entities.CollectionChanged += (sender, args) => { OnPropertyChanged(nameof(ParameterEntities)); };
                _list_package_entities.CollectionChanged += (sender, args) => { OnPropertyChanged(nameof(PackageEntities)); };
                _rolist_names = new ReadOnlyObservableList<CName>(_list_names);
                _rolist_parameter_entities = new ReadOnlyObservableList<CParameter.CParameterEntity>(_list_parameter_entities);
                _rolist_package_entities = new ReadOnlyObservableList<CPackage.CPackageEntity>(_list_package_entities);
                _rolist_datasheets = new ReadOnlyObservableList<CDatasheet>(_list_datasheets);
            }

            public void RemoveThis() {
                foreach (CName _element_entity_name in _list_names.ToArray()) { NameRemove(_element_entity_name.Name); }
                _element_item.ElementEntityRemove(this);
            }

            private void MakeNameTotal() {
                StringBuilder _sb = new StringBuilder(); bool _is_not_first = false;
                foreach (CName _entity_name in Names) { if (_is_not_first) _sb.Append(';'); else _is_not_first = true; _sb.Append(_entity_name.Name); }
                _name_total = _sb.ToString();
                OnPropertyChanged(nameof(NameTotal));
            }

            protected internal void OnPropertyChanged(string property_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(property_name)); }

            private int NameFind([NotNull] string name) {
                if (name == null) throw new ArgumentNullException("name");
                for (int _i = 0; _i < _list_names.Count; _i++) {
                    if (_list_names[_i]._name == name) return _i;
                }
                return -1;
            }
            public void NameAdd([NotNull] string name) {
                if (name == null) throw new ArgumentNullException("name");
                name = name.ToUpper();
                CName _element_entity_name = new CName(name, this);
                _element_item.NameAdd(_element_entity_name); _list_names.Add(_element_entity_name);
            }
            public void NameRemove([NotNull] string name) {
                if (name == null) throw new ArgumentNullException("name");
                name = name.ToUpper();
                _element_item.NameRemove(name); _list_names.RemoveAt(NameFind(name));
            }
            public void NameRename([NotNull] string old_name, [NotNull] string new_name) {
                if (old_name == null) throw new ArgumentNullException("old_name");
                if (new_name == null) throw new ArgumentNullException("new_name");
                old_name = old_name.ToUpper(); new_name = new_name.ToUpper();
                _element_item.NameRename(old_name, new_name); _list_names[NameFind(old_name)]._name = new_name;
                MakeNameTotal();
            }

            public CParameter.CParameterEntity ParameterGet([NotNull] string name, bool create_if_absent, string value) {
                if (name == null) throw new ArgumentNullException("name");
                try {
                    CParameter _parameter = _element_item._parameters.First(x => x.ParameterName.Equals(name, StringComparison.OrdinalIgnoreCase));
                    CParameter.CParameterEntity _parameter_entity;
                    if (!_dict_parameter_entities.TryGetValue(_parameter.ParameterName, out _parameter_entity)) {
                        if (create_if_absent) {
                            _parameter_entity = _parameter.FactoryEntity(this, value); value = null;
                            _dict_parameter_entities[_parameter.ParameterName] = _parameter_entity; _list_parameter_entities.Add(_parameter_entity);
                        }
                    }
                    if (_parameter_entity != null && value != null) _parameter_entity.StringValue = value;
                    return _parameter_entity;
                }
                catch (InvalidOperationException _ex_invalid_operation_exception) { throw new ArgumentException("Invalid parameter name", "name", _ex_invalid_operation_exception); }
            }
            public void ParameterRemove([NotNull] string name) {
                if (name == null) throw new ArgumentNullException("name");
                try {
                    CParameter _parameter = _element_item._parameters.First(x => x.ParameterName.Equals(name, StringComparison.OrdinalIgnoreCase));
                    CParameter.CParameterEntity _parameter_entity;
                    if (!_dict_parameter_entities.TryGetValue(_parameter.ParameterName, out _parameter_entity)) throw new InvalidOperationException("Parameter not found");
                    _dict_parameter_entities.Remove(_parameter.ParameterName); _list_parameter_entities.Remove(_parameter_entity);
                }
                catch (InvalidOperationException _ex_invalid_operation_exception) { throw new ArgumentException("Invalid parameter name", "name", _ex_invalid_operation_exception); }
            }

            public CPackage.CPackageEntity PackageGet([NotNull] string name, bool create_if_absent) {
                if (name == null) throw new ArgumentNullException("name");
                CPackage _package = CPackage.PackageGet(name); CPackage.CPackageEntity _package_entity;
                if (!_dict_package_entities.TryGetValue(_package.PackageName, out _package_entity)) {
                    if (create_if_absent) { _package_entity = new CPackage.CPackageEntity(_package, 0); _dict_package_entities[_package.PackageName] = _package_entity; _list_package_entities.Add(_package_entity); }
                }
                return _package_entity;
            }
            public void PackageRemove([NotNull] string name) {
                if (name == null) throw new ArgumentNullException("name");
                CPackage _package = CPackage.PackageGet(name);
                CPackage.CPackageEntity _package_entity;
                if (!_dict_package_entities.TryGetValue(_package.PackageName, out _package_entity)) throw new InvalidOperationException("Package not found");
                _dict_package_entities.Remove(_package.PackageName); _list_package_entities.Remove(_package_entity);
            }

            public CDatasheet DatasheetAdd([NotNull] string file_name, uint page) {
                if (file_name == null) throw new ArgumentNullException("file_name");
                CDatasheet _datasheet = new CDatasheet(file_name, page); _list_datasheets.Add(_datasheet);
                return _datasheet;
            }
            public void DatasheetRemove([NotNull] CDatasheet datasheet) {
                if (datasheet == null) throw new ArgumentNullException("datasheet");
                _list_datasheets.Remove(datasheet);
            }

            public bool Equals(CElementEntity other) { if (other == null) return false; return _guid == other._guid; }
            public int CompareTo(object obj) { return CompareTo(obj as CElementEntity); }
            public int CompareTo(CElementEntity other) { if (other == null) return 1; return _name_total.CompareTo(other._name_total); }

            //    **************
            public class CDatasheet : INotifyPropertyChanged, IEquatable<CDatasheet>, IComparable<CDatasheet>, IComparable
            {
                public event PropertyChangedEventHandler PropertyChanged;

                private string _filename;
                private uint _page;

                public string Filename { get { return _filename; } [NotNull]set { if (value == null) throw new ArgumentNullException("value"); _filename = value; OnPropertyChanged(nameof(Filename)); } }
                public uint Page { get { return _page; } set { _page = value; OnPropertyChanged(nameof(Page)); } }

                internal CDatasheet([NotNull] string filename, uint page) {
                    if (filename == null) throw new ArgumentNullException("filename");
                    _filename = filename; _page = page; PropertyChanged = null;
                }

                private void OnPropertyChanged(string property_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(property_name)); }

                public bool Equals(CDatasheet other) { if (other == null) return false; return string.Equals(_filename, other._filename, StringComparison.OrdinalIgnoreCase); }
                public int CompareTo(object obj) { return CompareTo(obj as CDatasheet); }
                public int CompareTo(CDatasheet other) { if (other == null) return 1; return string.Compare(_filename, other._filename, StringComparison.OrdinalIgnoreCase); }
            }
            //    **************
            public class CName : INotifyPropertyChanged, IEquatable<CName>, IComparable<CName>, IComparable
            {
                public event PropertyChangedEventHandler PropertyChanged;

                internal string _name;
                internal readonly CElementEntity _element_entity;

                public string Name { get { return _name; } set { _element_entity.NameRename(_name, value); OnPropertyChanged(nameof(Name)); } }
                public CElementEntity ElementEntity { get { return _element_entity; } }

                internal CName(string name, CElementEntity element_entity) { _name = name; _element_entity = element_entity; }

                public void RemoveThis() { _element_entity.NameRemove(_name); }

                protected internal void OnPropertyChanged(string property_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(property_name)); }

                public bool Equals(CName other) { if (other == null) return false; return string.Equals(_name, other._name); }
                public int CompareTo(object obj) { return CompareTo(obj as CName); }
                public int CompareTo(CName other) { if (other == null) return 1; return string.CompareOrdinal(_name, other._name); }
            }
            //    **************
        }
        //    **************
        public class CPackage : INotifyPropertyChanged, IEquatable<CPackage>, IComparable<CPackage>, IComparable
        {
            public event PropertyChangedEventHandler PropertyChanged;

            private static readonly Guid __GUID_TMP = Guid.NewGuid();
            private static readonly uint __CURRENT_VERSION = 1;
            private static readonly Dictionary<string, CPackage> __DICT_PACKAGES = new Dictionary<string, CPackage>();
            private static readonly ObservableSortedList<CPackage> __LIST_PACKAGES = new ObservableSortedList<CPackage>();
            private static readonly ReadOnlyObservableList<CPackage> __ROLIST_PACKAGES = new ReadOnlyObservableList<CPackage>(__LIST_PACKAGES);

            private readonly ObservableList<CPackageImage> _list_images = new ObservableList<CPackageImage>();
            private readonly ReadOnlyObservableList<CPackageImage> _rolist_images;

            private string _package_name;

            public static ReadOnlyObservableList<CPackage> Packages { get { return __ROLIST_PACKAGES; } }

            public ReadOnlyObservableList<CPackageImage> PackageImages { get { return _rolist_images; } }
            public string PackageName { get { return _package_name; } set { if (value == null) throw new ArgumentNullException("value"); PackageRename(value); } }

            private CPackage(string name) {
                _package_name = name.ToUpper();
                _rolist_images = new ReadOnlyObservableList<CPackageImage>(_list_images);
            }

            protected internal void OnPropertyChanged(string property_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(property_name)); }

            public static CPackage PackageGet([NotNull] string name) {
                if (name == null) throw new ArgumentNullException("name");
                name = name.ToUpper();
                CPackage _package; __DICT_PACKAGES.TryGetValue(name, out _package);
                if (_package == null) throw new ArgumentException(string.Format("No package named {0}", name));
                return _package;
            }
            public static CPackage PackageAdd([NotNull] string name) {
                if (name == null) throw new ArgumentNullException("name");
                name = name.ToUpper();
                CPackage _package = new CPackage(name);
                if (__DICT_PACKAGES.ContainsKey(name)) throw new ArgumentException(string.Format("Package {0} already exists", name));
                __DICT_PACKAGES[name] = _package; __LIST_PACKAGES.Add(_package);
                return _package;
            }
            public static void PackageRemove([NotNull] string name, [NotNull] CEtaElementsDatabase elements_database, [NotNull] Func<List<CElementEntity>, CPackage> func_replace_package) {
                if (name == null) throw new ArgumentNullException("name");
                if (elements_database == null) throw new ArgumentNullException("elements_database");
                if (func_replace_package == null) throw new ArgumentNullException("func_replace_package");
                if (__LIST_PACKAGES.Count < 2) throw new InvalidOperationException("Cannot remove all packages. At least one must be present.");
                name = name.ToUpper();
                CPackage _package;
                if (!__DICT_PACKAGES.TryGetValue(name, out _package)) throw new ArgumentException(string.Format("Package {0} not found", name));
                List<CElementEntity> _list_element_entities_to_replace = elements_database.EntitiesDistinct.Where(element => element.PackageEntities.Any(package_entity => package_entity.Package == _package)).ToList();
                if (_list_element_entities_to_replace.Count > 0) {
                    CPackage _package_to_replace = func_replace_package(_list_element_entities_to_replace);
                    if (_package_to_replace == null) return;
                    _list_element_entities_to_replace.ForEach(element_entity => { uint _count = element_entity.PackageGet(_package.PackageName, false).Count; element_entity.PackageRemove(_package.PackageName); element_entity.PackageGet(_package_to_replace.PackageName, true).Count = _count; });
                }
                __DICT_PACKAGES.Remove(name); __LIST_PACKAGES.Remove(_package);
            }
            public void PackageRename([NotNull] string new_name) {
                if (new_name == null) throw new ArgumentNullException("new_name");
                new_name = new_name.ToUpper();
                if (__DICT_PACKAGES.ContainsKey(new_name)) throw new ArgumentException(string.Format("Package {0} already exists", new_name));
                __DICT_PACKAGES.Remove(_package_name); __DICT_PACKAGES.Add(new_name, this);
                _package_name = new_name; OnPropertyChanged(nameof(PackageName));
            }

            public CPackageImage ImageAdd([NotNull] string filename) {
                if (filename == null) throw new ArgumentNullException("filename");
                CPackageImage _package_image = new CPackageImage(this, filename); _list_images.Add(_package_image);
                return _package_image;
            }
            public void ImageRemove([NotNull] CPackageImage package_image) {
                if (package_image == null) throw new ArgumentNullException("package_image");
                _list_images.Remove(package_image);
            }

            private static void ConvertVersion(XElement xml_element, uint old_version) {

            }

            public static XElement XMLGenerate() {
                XElement _xml_packages = new XElement("Packages"); _xml_packages.Add(new XAttribute("Version", __CURRENT_VERSION.ToString()));

                foreach (CPackage _package in __DICT_PACKAGES.Values) {
                    XElement _xml_package = new XElement("Package"); _xml_packages.Add(_xml_package);
                    _xml_package.Add(new XAttribute("Name", _package._package_name));
                    StringBuilder _sb = new StringBuilder(); bool _is_not_first = false; foreach (string _entity_name in _package._list_images.Select(x => x.Filename)) { if (_is_not_first) _sb.Append(';'); else _is_not_first = true; _sb.Append(_entity_name); };
                    _xml_package.Add(new XAttribute("Image", _sb.ToString()));
                }
                return _xml_packages;
            }

            public static void XMLLoad() {
                XDocument _xml_document = XDocument.Load(@"Packages\packages.xml");
                XElement _xml_packages = _xml_document.Element("Packages");
                if (_xml_packages == null) throw new ArgumentException("No Packages element");

                XAttribute _attribute_version = _xml_packages.Attribute("Version");
                if (_attribute_version == null) throw new ArgumentException("No Version attribute");
                uint _version = uint.Parse(_attribute_version.Value);
                if (_version > __CURRENT_VERSION) throw new ArgumentException("EtaElementsDatabase version is higher than supported by current version of Software. Please Update software");
                if (_version < __CURRENT_VERSION) ConvertVersion(_xml_packages, _version);

                foreach (XElement _xml_package in _xml_packages.Elements("Package")) {
                    XAttribute _attribute_name = _xml_package.Attribute("Name"); if (_attribute_name == null) throw new ArgumentException("No Name attribute");
                    string _name = _attribute_name.Value;
                    XAttribute _attribute_image = _xml_package.Attribute("Image"); if (_attribute_image == null) throw new ArgumentException("No Image attribute");
                    CPackage _package = PackageAdd(_name);
                    foreach (string _image_filename in _attribute_image.Value.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries)) { _package.ImageAdd(System.IO.Path.GetFileName(_image_filename).ToUpper()); }
                }
            }
            public static void XMLSave() {
                XDocument _xml_document = new XDocument(new XDeclaration("1.0", "windows-1251", null), XMLGenerate());

                try { File.Delete("Packages\\packages_backup10.xml"); }
                catch (Exception) { }
                for (int _i = 9; _i >= 0; _i--) {
                    try { File.Move(string.Format("Packages\\packages_backup{0:00}.xml", _i), string.Format("Packages\\packages_backup{0:00}.xml", _i + 1)); }
                    catch (Exception) { }
                }
                try { File.Move("Packages\\packages.xml", "Packages\\packages_backup00.xml"); }
                catch (Exception) { }
                _xml_document.Save("Packages\\packages.xml");
            }
            public static void XMLSaveTemp() {
                XDocument _xml_document = new XDocument(new XDeclaration("1.0", "windows-1251", null), XMLGenerate());
                _xml_document.Save(string.Format("Packages\\packages_{0:N}.xml", __GUID_TMP));
            }
            public static void XMLDeleteTemp() {
                try { File.Delete(string.Format("Packages\\packages_{0:N}.xml", __GUID_TMP)); }
                catch (Exception) { }
            }

            public bool Equals(CPackage other) { if (other == null) return false; return _package_name.Equals(other._package_name); }
            public int CompareTo(object obj) { return CompareTo(obj as CPackage); }
            public int CompareTo(CPackage other) { if (other == null) return 1; return _package_name.CompareTo(other._package_name); }

            //    **************
            public class CPackageImage
            {
                private readonly CPackage _package;
                private readonly string _filename;
                private readonly WeakReference<BitmapImage> _wr_image;

                public CPackage Package { get { return _package; } }
                public string Filename { get { return _filename; } }
                public BitmapImage Image { get { BitmapImage _img; if (!_wr_image.TryGetTarget(out _img)) { _img = new BitmapImage(new Uri(System.IO.Path.GetFullPath(System.IO.Path.Combine("Packages", _filename)))); _wr_image.SetTarget(_img); } return _img; } }

                internal CPackageImage(CPackage package, string filename) { _package = package; _filename = filename; _wr_image = new WeakReference<BitmapImage>(null); }
            }
            //    **************
            public class CPackageEntity : INotifyPropertyChanged, IEquatable<CPackageEntity>, IComparable<CPackageEntity>, IComparable
            {
                public event PropertyChangedEventHandler PropertyChanged;

                private readonly CPackage _package;
                private uint _count;

                public CPackage Package { get { return _package; } }
                public uint Count { get { return _count; } set { _count = value; OnPropertyChanged(nameof(Count)); } }

                public CPackageEntity(CPackage package, uint count) { _package = package; _count = count; }

                protected internal void OnPropertyChanged(string property_name) { PropertyChangedEventHandler _handler = PropertyChanged; if (_handler != null) _handler(this, new PropertyChangedEventArgs(property_name)); }

                public bool Equals(CPackageEntity other) { if (other == null) return false; return _package.Equals(other._package) && _count == other._count; }
                public int CompareTo(object obj) { return CompareTo(obj as CPackageEntity); }
                public int CompareTo(CPackageEntity other) {
                    int _res = 1;
                    if (other != null) {
                        _res = _package.CompareTo(other._package);
                        if (_res == 0) _res = _count.CompareTo(other._count);
                    }
                    return _res;
                }
            }
            //    **************
        }
    }
    //****************************************************************************************************************************************************
}
