IpcManagerRegisterNamespace("my.namespace");

while (IpcManagerHasMassage("my.namespace") == FALSE)
{
	Yield();
}

// TODO Get and handle message

IpcManagerCloseNamespace("my.namespace");