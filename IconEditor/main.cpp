#pragma pack(2)
#include "icon_editor.h"


int main(int argc, char *argv[])
{
	IconEditor* editor = new IconEditor;
	editor->run();
	delete editor;

	return 0;
}
