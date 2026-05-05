# generated from rosidl_generator_py/resource/_idl.py.em
# with input from slam_msgs:msg/PoseGraph.idl
# generated code does not contain a copyright notice


# Import statements for member types

# Member 'node_poses'
import array  # noqa: E402, I100

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_PoseGraph(type):
    """Metaclass of message 'PoseGraph'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('slam_msgs')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'slam_msgs.msg.PoseGraph')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__pose_graph
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__pose_graph
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__pose_graph
            cls._TYPE_SUPPORT = module.type_support_msg__msg__pose_graph
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__pose_graph

            from slam_msgs.msg import PoseEdge
            if PoseEdge.__class__._TYPE_SUPPORT is None:
                PoseEdge.__class__.__import_type_support__()

            from std_msgs.msg import Header
            if Header.__class__._TYPE_SUPPORT is None:
                Header.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class PoseGraph(metaclass=Metaclass_PoseGraph):
    """Message class 'PoseGraph'."""

    __slots__ = [
        '_header',
        '_node_poses',
        '_num_nodes',
        '_edges',
    ]

    _fields_and_field_types = {
        'header': 'std_msgs/Header',
        'node_poses': 'sequence<double>',
        'num_nodes': 'int32',
        'edges': 'sequence<slam_msgs/PoseEdge>',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.NamespacedType(['std_msgs', 'msg'], 'Header'),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.BasicType('double')),  # noqa: E501
        rosidl_parser.definition.BasicType('int32'),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.NamespacedType(['slam_msgs', 'msg'], 'PoseEdge')),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        from std_msgs.msg import Header
        self.header = kwargs.get('header', Header())
        self.node_poses = array.array('d', kwargs.get('node_poses', []))
        self.num_nodes = kwargs.get('num_nodes', int())
        self.edges = kwargs.get('edges', [])

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.header != other.header:
            return False
        if self.node_poses != other.node_poses:
            return False
        if self.num_nodes != other.num_nodes:
            return False
        if self.edges != other.edges:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def header(self):
        """Message field 'header'."""
        return self._header

    @header.setter
    def header(self, value):
        if __debug__:
            from std_msgs.msg import Header
            assert \
                isinstance(value, Header), \
                "The 'header' field must be a sub message of type 'Header'"
        self._header = value

    @builtins.property
    def node_poses(self):
        """Message field 'node_poses'."""
        return self._node_poses

    @node_poses.setter
    def node_poses(self, value):
        if isinstance(value, array.array):
            assert value.typecode == 'd', \
                "The 'node_poses' array.array() must have the type code of 'd'"
            self._node_poses = value
            return
        if __debug__:
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 all(isinstance(v, float) for v in value) and
                 all(not (val < -1.7976931348623157e+308 or val > 1.7976931348623157e+308) or math.isinf(val) for val in value)), \
                "The 'node_poses' field must be a set or sequence and each value of type 'float' and each double in [-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000, 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000]"
        self._node_poses = array.array('d', value)

    @builtins.property
    def num_nodes(self):
        """Message field 'num_nodes'."""
        return self._num_nodes

    @num_nodes.setter
    def num_nodes(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'num_nodes' field must be of type 'int'"
            assert value >= -2147483648 and value < 2147483648, \
                "The 'num_nodes' field must be an integer in [-2147483648, 2147483647]"
        self._num_nodes = value

    @builtins.property
    def edges(self):
        """Message field 'edges'."""
        return self._edges

    @edges.setter
    def edges(self, value):
        if __debug__:
            from slam_msgs.msg import PoseEdge
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 all(isinstance(v, PoseEdge) for v in value) and
                 True), \
                "The 'edges' field must be a set or sequence and each value of type 'PoseEdge'"
        self._edges = value
