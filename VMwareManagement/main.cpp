#include "VMware.h"
#include "VirtualMachine.h"

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#include <Windows.h>

#include <vix.h>


#define VMXPATH_INFO "D:\\VMware\\MacOSX_10.15.6\\macOS 10.15.vmx"
#define VM_USERNAME "tuannguyen"
#define VM_PASSWORD "test"
#define VM_SNAPSHOT "CleanTest"


int runProgramOnVirtualMachine(VixHandle vmHandle, const std::string& sProgramPath, const std::string& sArg)
{
    std::cout << sProgramPath + sArg << std::endl;
    VixHandle jobHandle = VixVM_RunProgramInGuest(
        vmHandle,
        sProgramPath.c_str(),
        sArg.c_str(),
        VIX_RUNPROGRAM_RETURN_IMMEDIATELY | VIX_RUNPROGRAM_ACTIVATE_WINDOW, // options,
        VIX_INVALID_HANDLE, // propertyListHandle,
        NULL,
        NULL);

    VixError err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_OK != err)
    {
        Vix_ReleaseHandle(jobHandle);
        return -1;
    }
    Vix_ReleaseHandle(jobHandle);

    return 0;
}

int powrOffMachine(VixHandle vmHandle)
{

}

int
main(int argc, char** argv)
{
    std::cout << "Start" << std::endl;
    VixError err = VIX_OK;
    VixHandle hostHandle = VIX_INVALID_HANDLE;
    VixHandle jobHandle = VIX_INVALID_HANDLE;
    VixHandle vmHandle = VIX_INVALID_HANDLE;
    VixHandle snapshotHandle = VIX_INVALID_HANDLE;

    int isLoggedIn = 0;

    jobHandle = VixHost_Connect(VIX_API_VERSION,
        VIX_SERVICEPROVIDER_VMWARE_WORKSTATION,
        NULL, // hostName
        0, // hostPort
        NULL, // userName
        NULL, // password
        0, // options
        VIX_INVALID_HANDLE, // propertyListHandle
        NULL, // callbackProc
        NULL); // clientData

    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_JOB_RESULT_HANDLE,
        &hostHandle,
        VIX_PROPERTY_NONE);

    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "Connect failed!" << std::endl;
        goto abort;
    }
    std::cout << "Connect successful!" << std::endl;
    Vix_ReleaseHandle(jobHandle);



    jobHandle = VixVM_Open(hostHandle,
        VMXPATH_INFO,
        NULL, // callbackProc
        NULL); // clientData
    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_JOB_RESULT_HANDLE,
        &vmHandle,
        VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        std::cout << "Open failed!" << std::endl;
        // Handle the error...
        goto abort;
    }
    std::cout << "Open successful!" << std::endl;
    Vix_ReleaseHandle(jobHandle);

    err = VixVM_GetNamedSnapshot(vmHandle, VM_SNAPSHOT, &snapshotHandle);
    if (VIX_OK != err)
    {
        // Handle the error...
        std::cout << "VixVM_GetNamedSnapshot failed!" << std::endl;
        goto abort;
    }



    jobHandle = VixVM_RevertToSnapshot(vmHandle,
        snapshotHandle,
        VIX_VMPOWEROP_LAUNCH_GUI, // options
        VIX_INVALID_HANDLE, // propertyListHandle
        NULL, // callbackProc
        NULL); // clientData

    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);

    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "VixVM_RevertToSnapshot failed!" << std::endl;
        goto abort;
    }
    std::cout << "VixVM_RevertToSnapshot successful!" << std::endl;0;
    Vix_ReleaseHandle(jobHandle);



    // Power on the virtual machine before copying file.
    jobHandle = VixVM_PowerOn(vmHandle,
        VIX_VMPOWEROP_LAUNCH_GUI, // powerOnOptions
        VIX_INVALID_HANDLE, // propertyListHandle
        NULL, // callbackProc
        NULL); // clientData

    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "VixVM_PowerOn failed!" << std::endl;
        goto abort;
    }
    std::cout << "VixVM_PowerOn successful!" << std::endl;
    Vix_ReleaseHandle(jobHandle);



    // Wait until guest is completely booted.
    std::cout << "Wait for booting ..." << std::endl;
    jobHandle = VixVM_WaitForToolsInGuest(vmHandle,
        300, // timeoutInSeconds
        NULL, // callbackProc
        NULL); // clientData

    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "VixVM_WaitForToolsInGuest failed!" << std::endl;
        goto abort;
    }
    std::cout << "VixVM_WaitForToolsInGuest successful!" << std::endl;
    Vix_ReleaseHandle(jobHandle);
    std::cout << "Boot - done" << std::endl;



    jobHandle = VixVM_AddSharedFolder(vmHandle,
        "sharedFolder",
        "C:\\Users\\tuan\\Desktop\\VMWareAPITest\\Resources\\",
        VIX_SHAREDFOLDER_WRITE_ACCESS,
        NULL, NULL);

    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_OK != err)
    {
        // Handle the error...
        std::cout << "VixVM_AddSharedFolder failed!" << std::endl;
        return -1;
    }
    else
        std::cout << "VixVM_AddSharedFolder successful!" << std::endl;


    jobHandle = VixVM_EnableSharedFolders(vmHandle,
        TRUE,
        0,
        NULL,
        NULL);

    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(jobHandle);
    if (VIX_OK != err)
    {
        // Handle the error...
        std::cout << "VixVM_EnableSharedFolders failed!" << std::endl;
        return -1;
    }
    else
        std::cout << "VixVM_EnableSharedFolders successful!" << std::endl;



    // Authenticate for guest operations.
    jobHandle = VixVM_LoginInGuest(vmHandle,
        "tuannguyen", // userName
        "test", // password
        0, // options
        NULL, // callbackProc
        NULL); // clientData

    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "VixVM_LoginInGuest failed!" << std::endl;
        goto abort;
    }

    isLoggedIn = 1;
    std::cout << "VixVM_LoginInGuest successful!" << std::endl;
    Vix_ReleaseHandle(jobHandle);



    // Create folder 'Services'
    jobHandle = VixVM_CreateDirectoryInGuest(vmHandle,
        "/Users/tuannguyen/Desktop/Services",
        VIX_INVALID_HANDLE, // propertyListHan
        NULL, // callbackProc
        NULL);

    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "Creating folder 'TestingService failed!" << std::endl;
        goto abort;
    }
    else
        std::cout << "Creating folder 'TestingService successful!" << std::endl;



    // Copy the file.
    std::cout << "Copying product ..." << std::endl;
    jobHandle = VixVM_CopyFileFromHostToGuest(vmHandle,
            "C:\\Users\\tuan\\Desktop\\VMWareAPITest\\Resources\\abc\\",  // src name
            "/Users/tuannguyen/Desktop/Services/product/", // dest name
            0, // options
            VIX_INVALID_HANDLE, // propertyListHan
            NULL, // callbackProc
            NULL);
 
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "VixVM_CopyFileFromHostToGuest failed!" << std::endl;
        goto abort;
    }
    std::cout << "VixVM_CopyFileFromHostToGuest successful!" << std::endl;

    std::cout << "Copying OESIS ..." << std::endl;
    jobHandle = VixVM_CopyFileFromHostToGuest(vmHandle,
        "C:\\Users\\tuan\\Desktop\\VMWareAPITest\\Resources\\def\\",  // src name
        "/Users/tuannguyen/Desktop/Services/OESIS/", // dest name
        0, // options
        VIX_INVALID_HANDLE, // propertyListHan
        NULL, // callbackProc
        NULL); // clientData
 
    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "VixVM_CopyFileFromHostToGuest failed!" << std::endl;
        goto abort;
    }
    std::cout << "VixVM_CopyFileFromHostToGuest successful!" << std::endl;

    std::cout << "Copying others ..." << std::endl;
    jobHandle = VixVM_CopyFileFromHostToGuest(vmHandle,
        "C:\\Users\\tuan\\Desktop\\VMWareAPITest\\Resources\\others\\",  // src name
        "/Users/tuannguyen/Desktop/Services/", // dest name
        0, // options
        VIX_INVALID_HANDLE, // propertyListHan
        NULL, // callbackProc
        NULL); // clientData

    err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "VixVM_CopyFileFromHostToGuest failed!" << std::endl;
        goto abort;
    }
    std::cout << "VixVM_CopyFileFromHostToGuest successful!" << std::endl;


    // Run an applicatiom
    runProgramOnVirtualMachine(vmHandle, "/bin/cp", " -r /Users/tuannguyen/Desktop/Services /Users/tuannguyen/Desktop/Services_copy");


    // PowerOff
    jobHandle = VixVM_PowerOff(vmHandle,
        VIX_VMPOWEROP_NORMAL,//VIX_VMPOWEROP_FROM_GUEST, // powerOffOptions,
        NULL, // callbackProc,
        NULL); // clientData

    err = VixJob_Wait(jobHandle,
        VIX_PROPERTY_NONE);
    if (VIX_OK != err) {
        // Handle the error...
        std::cout << "VixVM_PowerOff failed!" << std::endl;
        goto abort;
    }
    std::cout << "VixVM_PowerOff successful!" << std::endl;

abort:
    Vix_ReleaseHandle(jobHandle);
    Vix_ReleaseHandle(snapshotHandle);
    Vix_ReleaseHandle(vmHandle);
    VixHost_Disconnect(hostHandle);

    VixHost_Disconnect(hostHandle);

    return 0;
}


