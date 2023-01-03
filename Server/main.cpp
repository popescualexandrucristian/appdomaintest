#include <cstdint>
#include <stdlib.h>
#include <stdio.h>

public ref class ServerContext : public AppDomainTest::IContext, public System::MarshalByRefObject
{
public:
	virtual property System::String^ Owner;
	virtual property System::String^ Data;

	virtual void Call(System::String^% info)
	{
		System::IntPtr ptrToNativeString = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(info);
		const char* str = static_cast<const char*>(ptrToNativeString.ToPointer());
		printf("A call from the plugin \"%s\"(%p, %p) was made to the c++/CLI land from a sandboxed plugin.\n", str, str, info);
		info = info->ToUpper();
	}
};

ref class TaskData
{
public:
	TaskData(ServerContext^ context, AppDomainTest::SandboxData^ sandboxData) :
		context(context), sandboxData(sandboxData)
	{
	}
	void ExecuteUntrustedCode()
	{
		sandboxData->Sandbox->ExecuteUntrustedCode(context);
	}
private:
	ServerContext^ context;
	AppDomainTest::SandboxData^ sandboxData;
};

int main(int argc, char** argv)
{
	if (argc != 2)
		return EXIT_FAILURE;

	const uint16_t port = static_cast<const uint16_t>(atoll(argv[1]));

	System::Net::Sockets::Socket^ listener = gcnew System::Net::Sockets::Socket(
		System::Net::Sockets::SocketType::Stream,
		System::Net::Sockets::ProtocolType::Tcp
	);

	System::Net::IPEndPoint^ serverAddress = gcnew System::Net::IPEndPoint(System::Net::IPAddress::Any, port);
	listener->Bind(serverAddress);
	listener->Listen(99);

	array<unsigned char>^ receivedData = gcnew array<unsigned char>(4096 * 1024);
	System::Runtime::Serialization::Formatters::Binary::BinaryFormatter^ binaryFormatter = gcnew System::Runtime::Serialization::Formatters::Binary::BinaryFormatter();
	System::Text::ASCIIEncoding^ aSCIIEncoding = gcnew System::Text::ASCIIEncoding();
	
	while (true)
	{
		System::Net::Sockets::Socket^ connection = listener->Accept();

		connection->Receive(receivedData, 0, sizeof(int), System::Net::Sockets::SocketFlags::None);
		int dataLength = System::BitConverter::ToInt32(receivedData, 0);
		connection->Receive(receivedData, 0, dataLength, System::Net::Sockets::SocketFlags::None);

		System::IO::MemoryStream^ memoryStream = gcnew System::IO::MemoryStream(receivedData);
		System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^ data =
			safe_cast<System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^>(binaryFormatter->Deserialize(memoryStream));

		System::Type^ pluginInterfaceType = AppDomainTest::IPlugin::typeid;

		ServerContext^ context;

		System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>^ untrustedPlugins =
			gcnew System::Collections::Generic::Dictionary<System::String^, array<unsigned char>^>();


		for each (auto byteStream in data)
		{
			if (byteStream.Key == "file_data")
			{
				System::String^ fileData = aSCIIEncoding->GetString(byteStream.Value);
				context = gcnew ServerContext();
				context->Owner = connection->RemoteEndPoint->ToString();
				context->Data = fileData;
			}
			else
				untrustedPlugins[byteStream.Key] = byteStream.Value;
		}

		//in real life we would create this once when the plug-ins are loaded and operate on it until the end of time.
		AppDomainTest::SandboxData sandbox = AppDomainTest::Sandbox::Create(untrustedPlugins);

		System::Console::Write("Data state : "); System::Console::WriteLine(context->Data);

		TaskData^ taskData = gcnew TaskData(context, sandbox);
		auto action = gcnew System::Action(taskData, &TaskData::ExecuteUntrustedCode);
		auto cancelationToken = gcnew System::Threading::CancellationTokenSource();
		auto task = System::Threading::Tasks::Task::Run(action, cancelationToken->Token);
		task->Wait(3000);
		if (!task->CompletedTask)
			cancelationToken->Cancel(true);

		System::Console::Write("Data state : "); System::Console::WriteLine(context->Data);

		//we would add create and destroy+unload under an interface.
		sandbox.Sponsor->Unregister(sandbox.Sandbox);
		//we don't want the unload of the sandbox to be in the responsibility of the sandbox as such permissions might allow plug-ins to do things outside of it's app domain.
		System::AppDomain::Unload(sandbox.AppDomain);
	}

	return EXIT_SUCCESS;
}