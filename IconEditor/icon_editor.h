#pragma once

class IconEditor
{
public:
	IconEditor();
	virtual ~IconEditor();

	void run();
	void exit();

private:
	bool isRunning_ = true;
};
