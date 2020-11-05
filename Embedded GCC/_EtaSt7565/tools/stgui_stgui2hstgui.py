#!/usr/bin/env python3
import os
import sys
import argparse
import textwrap
import uuid
import inflection
from enum import Enum


class ParseError(Exception):
    def __init__(self, line, column, message):
        self.line = line
        self.column = column
        self.message = message


class StGUI:
    class Alignment(Enum):
        Start = (0, 'ALIGNMENT_START')
        Center = (1, 'ALIGNMENT_CENTER')
        End = (2, 'ALIGNMENT_END')

    class AlignmentEx(Enum):
        Start = (0, 'ALIGNMENT_EX_START')
        Center = (1, 'ALIGNMENT_EX_CENTER')
        End = (2, 'ALIGNMENT_EX_END')
        Spread = (3, 'ALIGNMENT_EX_SPREAD')

    class ActionGroup:
        def __init__(self, action_group_name):
            self.action_group_name = action_group_name
            self.actions = []
            StGUI.Action(self, "_None")

        def generate_name(self):
            return f'eStGUI_ActionGroup_{self.action_group_name}'

    class Action:
        def __init__(self, action_group, action_name):
            self.action_group: StGUI.ActionGroup = action_group
            self.action_name = action_name
            self.action_group.actions.append(self)

        def generate_name(self):
            return f'ActionGroup_{self.action_group.action_group_name}_{self.action_name}'

    class DialogValue:
        def __init__(self, value, action):
            self.value = value
            self.action: StGUI.Action = action

        def get_pointer(self):
            return f'.index = STGUI_DIALOG_MAKE_POINTER({self.action.generate_name()})'

        def get_text(self):
            if self.action is not None:
                return self.get_pointer()
            elif self.value is None:
                return '.text = NULL'
            else:
                return f'.value.text = "{self.value}"'

        def get_bitmap(self):
            if self.action is not None:
                return self.get_pointer()
            else:
                return f'.value.bitmap = &g_stgui_bitmap_{self.value}'

        def get_progress(self):
            if self.action is not None:
                return self.get_pointer()
            else:
                return f'.value.progress = {self.value}'

    def code_symbol(self):
        ...

    def code_symbol_base(self):
        ...

    def get_action_groups(self, action_groups_dict):
        ...

    def generate_code(self, dest_hstgui):
        ...

    @staticmethod
    def parse_orientation(name, line, column):
        if name == 'v':
            return True
        elif name == 'h':
            return False
        else:
            raise ParseError(line, column, f'invalid orientation: "{name}"')

    @staticmethod
    def parse_alignment(name, line, column):
        try:
            return StGUI_Dialog_Container.Alignment[name]
        except Exception:
            raise ParseError(line, column, f'invalid alignment: "{name}"')

    @staticmethod
    def parse_alignment_ex(name, line, column):
        try:
            return StGUI_Dialog_Container.AlignmentEx[name]
        except Exception:
            raise ParseError(line, column, f'invalid extended alignment: "{name}"')

    def __init__(self):
        self.uuid = str(uuid.uuid4()).replace('-', '')


class StGUI_Container(StGUI):
    def __init__(self):
        super().__init__()
        self.items = []


class StGUI_Menu(StGUI_Container):
    def __init__(self, name, text):
        super().__init__()
        self.name = name
        self.text = text

    def code_symbol(self):
        return f'g_stgui_menu_{self.uuid}'

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'

    def generate_code(self, dest_hstgui):
        for i, item in enumerate(self.items):
            item.generate_code(dest_hstgui)
            if len(self.items) - 1 > i:
                dest_hstgui.write('\n')

        if len(self.items) > 0:
            dest_hstgui.write('\n')

        dest_hstgui.write(f'static sStGUI_MenuItemBase const *const g_sub_menu_items_{self.uuid}[] = {{\n')
        for i, item in enumerate(self.items):
            dest_hstgui.write(f'    &{item.code_symbol_base()}{"," if len(self.items) - 1 > i else ""}\n')
        dest_hstgui.write('};\n')

        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_Menu const {0} = {{
                .base.menu_item_type = MENU_ITEM_TYPE_MENU,
                .base.text = "{1}",
                .sub_items = g_sub_menu_items_{2},
                .sub_items_count = sizeof_array(g_sub_menu_items_{2})
            }};
            """.format(self.code_symbol(), self.text, self.uuid)))


class StGUI_Menu_Action(StGUI):
    def __init__(self, text, action: StGUI.Action):
        super().__init__()
        self.text = text
        self.action: StGUI.Action = action

    def code_symbol(self):
        return f'g_stgui_menu_action_{self.uuid}'

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_MenuItemAction const {0} = {{
                .base.menu_item_type = MENU_ITEM_TYPE_ACTION,
                .base.text = "{1}",
                .action_code = {2}
            }};
            """.format(self.code_symbol(), self.text, self.action.generate_name())))


class StGUI_Menu_Checkbox(StGUI):
    def __init__(self, text, variable):
        super().__init__()
        self.text = text
        self.variable = variable

    def code_symbol(self):
        return f'g_stgui_menu_checkbox_{self.uuid}'

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_MenuItemCheckbox const {0} = {{
                .base.menu_item_type = MENU_ITEM_TYPE_CHECKBOX,
                .base.text = "{1}",
                .check_value = &{2}
            }};
            """.format(self.code_symbol(), self.text, self.variable)))


class StGUI_Menu_Choices(StGUI):
    def __init__(self, text, variable):
        super().__init__()
        self.text = text
        self.variable = variable
        self.choices = []

    def code_symbol(self):
        return f'g_stgui_menu_choices_{self.uuid}'

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(f'static char const *const s_text_of_choices_{self.uuid}[] = {{\n')
        for i, choice in enumerate(self.choices):
            dest_hstgui.write(f'    "{choice}"{"," if len(self.choices) - 1 > i else ""}\n')
        dest_hstgui.write('};\n')

        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_MenuItemChoices const {0} = {{
                .base.menu_item_type = MENU_ITEM_TYPE_CHOICES,
                .base.text = "{1}",
                .text_choices = s_text_of_choices_{2},
                .text_choices_count = sizeof_array(s_text_of_choices_{2}),
                .choice_value = &{3}
            }};
            """.format(self.code_symbol(), self.text, self.uuid, self.variable)))


class StGUI_Dialog_Base(StGUI):
    def __init__(self):
        super().__init__()


class StGUI_Dialog_Single(StGUI_Dialog_Base):
    def __init__(self, child_item):
        super().__init__()
        self.child_item = child_item

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'

    def generate_code(self, dest_hstgui):
        self.child_item.generate_code(dest_hstgui)


class StGUI_Dialog_Container(StGUI_Dialog_Base, StGUI_Container):
    def __init__(self):
        super().__init__()

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'

    def generate_code(self, dest_hstgui):
        for i, item in enumerate(self.items):
            item.generate_code(dest_hstgui)
            if len(self.items) - 1 > i:
                dest_hstgui.write('\n')

        if len(self.items) > 0:
            dest_hstgui.write('\n')

        dest_hstgui.write(f'static sStGUI_DialogItemBase const *const g_sub_dialog_items_{self.uuid}[] = {{\n')
        for i, item in enumerate(self.items):
            dest_hstgui.write(f'    &{item.code_symbol_base()}{"," if len(self.items) - 1 > i else ""}\n')
        dest_hstgui.write('};\n')


class StGUI_Dialog_Text(StGUI_Dialog_Base):
    def __init__(self, text: StGUI.DialogValue, min_width: int, max_width: int):
        super().__init__()
        self.text: StGUI.DialogValue = text
        self.min_width: int = min_width
        self.max_width: int = max_width

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'


class StGUI_Dialog(StGUI_Dialog_Single):
    def __init__(self, caption: StGUI.DialogValue, child_item):
        super().__init__(child_item)
        self.caption: StGUI.DialogValue = caption

    def code_symbol(self):
        return f'g_stgui_dialog_{self.uuid}'

    def code_symbol_base(self):
        return f'{super().code_symbol_base()}.base'

    def generate_code(self, dest_hstgui):
        super().generate_code(dest_hstgui)

        dest_hstgui.write(textwrap.dedent("""\
            
            static sStGUI_Dialog const {0} = {{
                .base.base.dialog_item_type = DIALOG_ITEM_TYPE_SINGLE_DIALOG,
                .base.child_item = &{1},
                .dialog_pointer_caption{2}
            }};
            """.format(self.code_symbol(), self.child_item.code_symbol_base(), self.caption.get_text())))


class StGUI_Dialog_Separator(StGUI_Dialog_Base):
    def __init__(self, width: int, height: int):
        super().__init__()
        self.width: int = width
        self.height: int = height

    def code_symbol(self):
        return f'g_stgui_dialog_separator_{self.uuid}'

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_DialogItemSeparator const {0} = {{
                .base.dialog_item_type = DIALOG_ITEM_TYPE_PRIMITIVE_SEPARATOR,
                .width = {1},
                .height = {2}
            }};
            """.format(self.code_symbol(), self.width, self.height)))


class StGUI_Dialog_Bitmap(StGUI_Dialog_Base):
    def __init__(self, bitmap: StGUI.DialogValue, alignment_h: StGUI.Alignment, alignment_v: StGUI.Alignment):
        super().__init__()
        self.bitmap: StGUI.DialogValue = bitmap
        self.alignment_h: StGUI.Alignment = alignment_h
        self.alignment_v: StGUI.Alignment = alignment_v

    def code_symbol(self):
        return f'g_stgui_dialog_bitmap_{self.uuid}'

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_DialogItemBitmap const {0} = {{
                .base.dialog_item_type = DIALOG_ITEM_TYPE_PRIMITIVE_BITMAP,
                .dialog_pointer_bitmap{1},
                .alignment_horizontal = {2},
                .alignment_vertical = {3}
            }};
            """.format(self.code_symbol(), self.bitmap.get_bitmap(), self.alignment_h.value[1], self.alignment_v.value[1])))


class StGUI_Dialog_Progress(StGUI_Dialog_Base):
    def __init__(self, width: int, height: int, progress: StGUI.DialogValue):
        super().__init__()
        self.width: int = width
        self.height: int = height
        self.progress: StGUI.DialogValue = progress

    def code_symbol(self):
        return f'g_stgui_dialog_progress_{self.uuid}'

    def code_symbol_base(self):
        return f'{self.code_symbol()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_DialogItemProgress const {0} = {{
                .base.dialog_item_type = DIALOG_ITEM_TYPE_PRIMITIVE_PROGRESS,
                .width = {1},
                .height = {2},
                .dialog_pointer_progress{3}
            }};
            """.format(self.code_symbol(), self.width, self.height, self.progress.get_progress())))


class StGUI_Dialog_Label(StGUI_Dialog_Text):
    def __init__(self, text: StGUI.DialogValue, min_width: int, max_width: int, alignment: StGUI.Alignment):
        super().__init__(text, min_width, max_width)
        self.alignment: StGUI.Alignment = alignment

    def code_symbol(self):
        return f'g_stgui_dialog_text_{self.uuid}'

    def code_symbol_base(self):
        return f'{super().code_symbol_base()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_DialogItemLabel const {0} = {{
                .base.base.dialog_item_type = DIALOG_ITEM_TYPE_PRIMITIVE_LABEL,
                .base.dialog_pointer_text{1},
                .base.min_width = {2},
                .base.max_width = {3},
                .alignment = {4}
            }};
            """.format(self.code_symbol(), self.text.get_text(), self.min_width, self.max_width, self.alignment.value[1])))


class StGUI_Dialog_Button(StGUI_Dialog_Text):
    def __init__(self, text: StGUI.DialogValue, min_width: int, max_width: int, action: StGUI.Action):
        super().__init__(text, min_width, max_width)
        self.action: StGUI.Action = action

    def code_symbol(self):
        return f'g_stgui_dialog_button_{self.uuid}'

    def code_symbol_base(self):
        return f'{super().code_symbol_base()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_DialogItemButton const {0} = {{
                .base.base.dialog_item_type = DIALOG_ITEM_TYPE_PRIMITIVE_BUTTON,
                .base.dialog_pointer_text{1},
                .base.min_width = {2},
                .base.max_width = {3},
                .action_code = {4}
            }};
            """.format(self.code_symbol(), self.text.get_text(), self.min_width, self.max_width, self.action.generate_name())))


class StGUI_Dialog_Checkbox(StGUI_Dialog_Text):
    def __init__(self, text, min_width: int, max_width: int, action: StGUI.Action):
        super().__init__(text, min_width, max_width)
        self.action: StGUI.Action = action

    def code_symbol(self):
        return f'g_stgui_dialog_button_{self.uuid}'

    def code_symbol_base(self):
        return f'{super().code_symbol_base()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_DialogItemButton const {0} = {{
                .base.base.dialog_item_type = DIALOG_ITEM_TYPE_PRIMITIVE_BUTTON,
                .base.dialog_pointer_text{1},
                .base.min_width = {2},
                .base.max_width = {3},
                .action_code = {4}
            }};
            """.format(self.code_symbol(), self.text.get_text(), self.min_width, self.max_width, self.action.generate_name())))


class StGUI_Dialog_ProgressString(StGUI_Dialog_Text):
    def __init__(self, text: StGUI.DialogValue, min_width: int, max_width: int, progress: StGUI.DialogValue):
        super().__init__(text, min_width, max_width)
        self.progress: StGUI.DialogValue = progress

    def code_symbol(self):
        return f'g_stgui_dialog_progress_string_{self.uuid}'

    def code_symbol_base(self):
        return f'{super().code_symbol_base()}.base'

    def generate_code(self, dest_hstgui):
        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_DialogItemProgressString const {0} = {{
                .base.base.dialog_item_type = DIALOG_ITEM_TYPE_PRIMITIVE_PROGRESS_STRING,
                .base.dialog_pointer_text{1},
                .base.min_width = {2},
                .base.max_width = {3},
                .dialog_pointer_progress{4}
            }};
            """.format(self.code_symbol(), self.text.get_text(), self.min_width, self.max_width, self.progress.get_progress())))


class StGUI_Dialog_Spacer(StGUI_Dialog_Single):
    def __init__(self, child_item, space_left, space_right, space_top, space_bottom):
        super().__init__(child_item)
        self.space_left = space_left
        self.space_right = space_right
        self.space_top = space_top
        self.space_bottom = space_bottom

    def code_symbol(self):
        return f'g_stgui_dialog_single_spacer_{self.uuid}'

    def code_symbol_base(self):
        return f'{super().code_symbol_base()}.base'

    def generate_code(self, dest_hstgui):
        super().generate_code(dest_hstgui)

        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_DialogItemSpacer const {0} = {{
                .base.base.dialog_item_type = DIALOG_ITEM_TYPE_SINGLE_SPACER,
                .base.child_item = &{1},
                .space_left = {2},
                .space_right = {3},
                .space_top = {4},
                .space_bottom = {5}
            }};
            """.format(self.code_symbol(), self.child_item.code_symbol_base(), self.space_left, self.space_right, self.space_top, self.space_bottom)))


class StGUI_Dialog_Container_Stack(StGUI_Dialog_Container):
    class ContainerStackAlignment(Enum):
        SpreadStart = (0, 'DIALOG_CONTAINER_STACK_ALIGNMENT_SPREAD_START')
        Start = (1, 'DIALOG_CONTAINER_STACK_ALIGNMENT_START')
        Center = (2, 'DIALOG_CONTAINER_STACK_ALIGNMENT_CENTER')
        End = (3, 'DIALOG_CONTAINER_STACK_ALIGNMENT_END')
        SpreadEnd = (4, 'DIALOG_CONTAINER_STACK_ALIGNMENT_SPREAD_END')

    def __init__(self, is_vertical: bool, container_alignment: ContainerStackAlignment, array_alignment: StGUI.AlignmentEx, separator_length: int):
        super().__init__()
        self.is_vertical: bool = is_vertical
        self.container_alignment: StGUI_Dialog_Container_Stack.ContainerStackAlignment = container_alignment
        self.array_alignment: StGUI.AlignmentEx = array_alignment
        self.separator_length: int = separator_length

    @staticmethod
    def parse_container_alignment(name, line, column):
        try:
            return StGUI_Dialog_Container_Stack.ContainerStackAlignment[name]
        except Exception:
            raise ParseError(line, column, f'invalid container alignment: "{name}"')

    def code_symbol(self):
        return f'g_stgui_dialog_container_stack_{self.uuid}'

    def code_symbol_base(self):
        return f'{super().code_symbol_base()}.base'

    def generate_code(self, dest_hstgui):
        super().generate_code(dest_hstgui)

        dest_hstgui.write(textwrap.dedent("""\
            static sStGUI_DialogItemContainerStack const {0} = {{
                .base.base.dialog_item_type = DIALOG_ITEM_TYPE_CONTAINER_STACK,
                .base.child_items = g_sub_dialog_items_{1},
                .base.child_items_count = sizeof_array(g_sub_dialog_items_{1}),
                .is_vertical = {2},
                .container_alignment = {3},
                .array_alignment = {4},
                .separator_length = {5}
            }};
            """.format(self.code_symbol(), self.uuid, "true" if self.is_vertical else "false", self.container_alignment.value[1], self.array_alignment.value[1], self.separator_length)))


def log_error(text):
    print(text, file=sys.stderr)


def main(args):
    with open(args.stgui_script, "r") as script:
        line = 0
        column = 1
        line_str = ""

        # ===============================================================================
        def read_char(poke, allow_nextline):
            nonlocal line, column, line_str
            if line_str is None:
                return None
            while len(line_str) <= (column - 1):
                if not allow_nextline:
                    return None

                line += 1
                column = 1
                script.readable()
                line_str = script.readline()

                if not line_str:
                    line_str = None
                    return None

            char = line_str[column - 1]
            if not poke:
                column += 1
            return char

        # ===============================================================================
        def skip_whites():
            while True:
                char = read_char(True, True)
                if char is None or not char.isspace():
                    return
                read_char(False, True)

        # ===============================================================================
        def is_end_of_file():
            skip_whites()
            return read_char(True, True) is None

        # ===============================================================================
        def need_symbol(description, optional=False):
            symbol = ""

            skip_whites()
            sline = line
            scolumn = column
            is_first = True

            while True:
                char = read_char(True, False)
                if char is not None and (char == '_' or ((is_first and char.isalpha()) or char.isalnum())):
                    symbol += char
                    read_char(False, False)
                    is_first = False
                else:
                    if len(symbol) > 0:
                        return symbol
                    else:
                        if optional:
                            return None
                        else:
                            raise ParseError(sline, scolumn, f"{description} required")

        # ===============================================================================
        def need_integer(description, optional=False, action_group: StGUI.ActionGroup = None):
            skip_whites()
            if action_group is not None and read_char(True, False) == '*':
                read_char(False, False)
                return StGUI.DialogValue(None, StGUI.Action(action_group, need_symbol(f"{description} index")))

            integer = ""
            sline = line
            scolumn = column

            while True:
                char = read_char(True, False)
                if char is not None and char.isnumeric():
                    integer += char
                    read_char(False, False)
                else:
                    if len(integer) > 0:
                        if action_group is not None:
                            return StGUI.DialogValue(int(integer), None)
                        else:
                            return int(integer)
                    else:
                        if optional:
                            if action_group is not None:
                                return StGUI.DialogValue(None, None)
                            else:
                                return None
                        else:
                            raise ParseError(sline, scolumn, f"{description} required")

        # ===============================================================================
        def need_text(description, optional=False, action_group: StGUI.ActionGroup = None):
            skip_whites()
            if action_group is not None and read_char(True, False) == '*':
                read_char(False, False)
                return StGUI.DialogValue(None, StGUI.Action(action_group, need_symbol(f"{description} index")))

            text = ""
            sline = line
            scolumn = column

            if read_char(True, False) != '"':
                if optional:
                    if action_group is not None:
                        return StGUI.DialogValue(None, None)
                    else:
                        return None
                else:
                    raise ParseError(sline, scolumn, f'{description} required')

            read_char(False, False)

            extra_char = False
            while True:
                char = read_char(False, False)
                if not extra_char and char == '"':
                    if action_group is not None:
                        return StGUI.DialogValue(text, None)
                    else:
                        return text
                elif char is None:
                    raise ParseError(sline, scolumn, f'unfinished {description}')

                extra_char = char == '\\'
                text += char

        # ===============================================================================
        def need_variable(description, optional=False, action_group: StGUI.ActionGroup = None):
            skip_whites()
            if action_group is not None and read_char(True, False) == '*':
                read_char(False, False)
                return StGUI.DialogValue(None, StGUI.Action(action_group, need_symbol(f"{description} index")))

            variable = ""
            sline = line
            scolumn = column

            while True:
                char = read_char(True, False)
                if char is None or char.isspace():
                    if len(variable) > 0:
                        if action_group is not None:
                            return StGUI.DialogValue(variable, None)
                        else:
                            return variable
                    else:
                        if optional:
                            if action_group is not None:
                                return StGUI.DialogValue(variable, None)
                            else:
                                return None
                        else:
                            raise ParseError(sline, scolumn, f'{description} required')

                read_char(False, False)
                variable += char

        # ===============================================================================
        def need_directive():
            skip_whites()
            sline = line
            scolumn = column
            directive = need_symbol("directive")
            if read_char(False, False) != ':':
                raise ParseError(sline, scolumn, 'directive must end with ":" symbol')

            return directive

        # ===============================================================================
        def need_orientation(description):
            skip_whites()
            sline = line
            scolumn = column
            return StGUI.parse_orientation(need_symbol(f"{description} orientation"), sline, scolumn)

        # ===============================================================================
        def need_alignment(description):
            skip_whites()
            sline = line
            scolumn = column
            return StGUI.parse_alignment(need_symbol(f"{description} alignment"), sline, scolumn)

        # ===============================================================================
        def need_alignment_ex(description):
            skip_whites()
            sline = line
            scolumn = column
            return StGUI.parse_alignment_ex(need_symbol(f"{description} extended alignment"), sline, scolumn)

        # ===============================================================================
        def need_container_stack_alignment():
            skip_whites()
            sline = line
            scolumn = column
            return StGUI_Dialog_Container_Stack.parse_container_alignment(need_symbol(f"container alignment"), sline, scolumn)

        # ===============================================================================
        def parse_body(block):
            skip_whites()
            sline = line
            scolumn = column
            if read_char(False, True) != '{':
                raise ParseError(sline, scolumn, 'required "{" symbol')

            block()

            skip_whites()
            sline = line
            scolumn = column
            if read_char(False, True) != '}':
                raise ParseError(sline, scolumn, 'required "}" symbol')

        # ===============================================================================
        def is_end_of_body():
            skip_whites()
            return read_char(True, True) == '}'

        # ===============================================================================
        def parse_menu(action_group: StGUI.ActionGroup):
            name = need_symbol("menu name", True)
            text = need_text("menu text")
            menu = StGUI_Menu(name, text)
            parse_body(lambda: parse_body_container(menu, action_group))
            return menu

        # ===============================================================================
        def parse_body_menu_choices(parent: StGUI_Menu_Choices):
            while True:
                if is_end_of_body():
                    return parent

                text = need_text("choice text")
                parent.choices.append(text)

        # ===============================================================================
        def parse_menu_item(action_group: StGUI.ActionGroup):
            skip_whites()
            sline = line
            scolumn = column
            directive = need_directive()

            if directive == 'menu':
                current = parse_menu(action_group)

            elif directive == 'action':
                text = need_text("action text")
                action_name = need_symbol("action name")
                current = StGUI_Menu_Action(text, StGUI.Action(action_group, action_name))

            elif directive == 'checkbox':
                text = need_text("checkbox text")
                variable = need_variable("checkbox variable")
                current = StGUI_Menu_Checkbox(text, variable)

            elif directive == 'choices':
                text = need_text("choices text")
                variable = need_variable("choices variable")
                current = current_choices = StGUI_Menu_Choices(text, variable)
                parse_body(lambda: parse_body_menu_choices(current_choices))

            else:
                raise ParseError(sline, scolumn, f'forbidden directive of menu: "{directive}"')

            return current

        # ===============================================================================
        def parse_dialog_item(action_group: StGUI.ActionGroup):
            skip_whites()
            sline = line
            scolumn = column
            directive = need_directive()

            if directive == 'separator':
                width = need_integer("separator width")
                height = need_integer("separator height")
                current = StGUI_Dialog_Separator(width, height)

            elif directive == 'bitmap':
                bitmap = need_variable("bitmap", action_group=action_group)
                alignment_h = need_alignment("horizontal bitmap")
                alignment_v = need_alignment("vertical bitmap")
                current = StGUI_Dialog_Bitmap(bitmap, alignment_h, alignment_v)

            elif directive == 'progress':
                width = need_integer("progress width")
                height = need_integer("progress height")
                progress = need_integer("progress", action_group=action_group)
                current = StGUI_Dialog_Progress(width, height, progress)

            elif directive == 'progress_string':
                text = need_text("progress text", action_group=action_group)
                min_width = need_integer("progress minimum width", optional=True)
                max_width = need_integer("progress maximum width", optional=True)
                progress = need_integer("progress", action_group=action_group)
                current = StGUI_Dialog_ProgressString(text, 0 if min_width is None else min_width, 0 if max_width is None else max_width, progress)

            elif directive == 'text':
                text = need_text("label text", action_group=action_group)
                min_width = need_integer("label minimum width", optional=True)
                max_width = need_integer("label maximum width", optional=True)
                alignment = need_alignment("dialog text")
                current = StGUI_Dialog_Label(text, 0 if min_width is None else min_width, 0 if max_width is None else max_width, alignment)

            elif directive == 'button':
                text = need_text("button text", action_group=action_group)
                min_width = need_integer("button minimum width", optional=True)
                max_width = need_integer("button maximum width", optional=True)
                action_name = need_symbol("button action name")
                current = StGUI_Dialog_Button(text, 0 if min_width is None else min_width, 0 if max_width is None else max_width, StGUI.Action(action_group, action_name))

            elif directive == 'spacer':
                space_left = need_integer("space left")
                space_right = need_integer("space right")
                space_top = need_integer("space top")
                space_bottom = need_integer("space bottom")
                child_item = parse_dialog_item()
                current = StGUI_Dialog_Spacer(child_item, space_left, space_right, space_top, space_bottom)

            elif directive == 'container':
                is_vertical = need_orientation("container")
                container_alignment = need_container_stack_alignment()
                array_alignment = need_alignment_ex("container")
                separator_length = need_integer("container separator length", optional=True)
                current = current_container = StGUI_Dialog_Container_Stack(is_vertical, container_alignment, array_alignment, 0 if separator_length is None else separator_length)
                parse_body(lambda: parse_body_container(current_container, action_group))

            else:
                raise ParseError(sline, scolumn, f'forbidden directive of dialog: "{directive}"')

            return current

        # ===============================================================================
        def parse_body_container(parent: StGUI_Container, action_group: StGUI.ActionGroup):
            while True:
                if is_end_of_body():
                    return parent

                if isinstance(parent, StGUI_Menu):
                    current = parse_menu_item(action_group)

                elif isinstance(parent, StGUI_Dialog_Container):
                    current = parse_dialog_item(action_group)

                else:
                    raise ParseError(0, 0, f'invalid container "{type(parent)}"')

                parent.items.append(current)

        def parse_root():
            skip_whites()
            sline = line
            scolumn = column
            directive = need_directive()

            if directive == 'dialog':
                action_group = StGUI.ActionGroup(need_symbol("dialog action group"))
                caption = need_text("dialog caption", optional=True, action_group=action_group)
                item = parse_dialog_item(action_group)
                root = StGUI_Dialog(caption, item)

            elif directive == 'raw_dialog':
                action_group = StGUI.ActionGroup(need_symbol("dialog action group"))
                root = parse_dialog_item(action_group)

            elif directive == 'menu':
                action_group = StGUI.ActionGroup(need_symbol("menu action group"))
                root = parse_menu(action_group)

            else:
                raise ParseError(sline, scolumn, f'forbidden directive of root "{directive}"')

            return action_group, root

        # ===============================================================================
        try:
            roots = [parse_root()]
            while not is_end_of_file():
                roots.append(parse_root())

            basename = os.path.splitext(args.stgui_script)[0]
            hstgui = basename + ".hstgui"

            with open(hstgui, "w") as dest_hstgui:
                dest_hstgui.write(textwrap.dedent("""\
                    /// Generated with {0}
                    
                    #include "eta_stgui_menus.h"
                    #include "eta_stgui_dialogs.h"
                    
                    """.format(os.path.split(__file__)[1])))

                list_menus = []
                list_dialogs = []
                for action_group, root in roots:
                    if isinstance(root, StGUI_Menu):
                        list_menus.append((action_group, root))
                    if isinstance(root, StGUI_Dialog_Base):
                        list_dialogs.append((action_group, root))

                for menu in list_menus:
                    dest_hstgui.write(f'#define STGUI_MENU_{inflection.underscore(menu[0].action_group_name).upper()} {menu[1].code_symbol()}\n')
                if len(list_menus) > 0:
                    dest_hstgui.write('\n')

                for dialog in list_dialogs:
                    dest_hstgui.write(f'#define STGUI_DIALOG_{inflection.underscore(dialog[0].action_group_name).upper()} {dialog[1].code_symbol()}\n')
                if len(list_dialogs) > 0:
                    dest_hstgui.write('\n')

                for action_group, root in roots:
                    dest_hstgui.write('typedef enum {\n')
                    for i, action in enumerate(action_group.actions):
                        dest_hstgui.write(f'    {action.generate_name()}{"," if len(action_group.actions) - 1 > i else ""}\n')
                    dest_hstgui.write(f'}} eStGUI_ActionGroup_{action_group.generate_name()};\n\n')

                for i, root in enumerate(roots):
                    root[1].generate_code(dest_hstgui)
                    if i != len(roots) - 1:
                        dest_hstgui.write('\n')

        except ParseError as err:
            log_error(f'{args.stgui_script}:{err.line}:{err.column}: error: {err.message}')
            return -1

    return 0


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description="ST7565 GUI script to header converter")
    ap.add_argument("stgui_script", type=str, help="ST7565 GUI script filename")

    sys.exit(main(ap.parse_args()))
