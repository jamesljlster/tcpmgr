using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace netlib
{
    partial class tcpmgr
    {
        private void tcpmgr_client_thread(object obj)
        {
            tcpmgr_list listRef = (tcpmgr_list)obj;
            Monitor.Enter(listRef.cond);

            // Run client task
            listRef.client_task(listRef.usrData, listRef.clientSock);

            // Close socket
            listRef.clientSock.Close();
            listRef.sockStatus = 0;

            // Cleanup
            Monitor.Pulse(listRef.cond);
            listRef.closeJoin = 1;

            Monitor.Exit(listRef.cond);
        }

        private void tcpmgr_accept_thread(object obj)
        {
            tcpmgr mgrRef = (tcpmgr)obj;

            while (mgrRef.stopFlag == 0)
            {
                Console.WriteLine("Waiting for client connection...");
                
                while(mgrRef.stopFlag == 0 && !mgrRef.listenSock.Pending())
                {
                    Thread.Sleep(100);
                }

                // Accept client
                Socket clientSock = mgrRef.listenSock.AcceptSocket();

                // Lock client list
                lock(mgrRef.mgrList)
                {
                    // Search empty entity
                    int tmpIndex = -1;
                    for(int i = 0; i < mgrRef.mgrList.Length; i++)
                    {
                        if(mgrRef.mgrList[i].occupied == 0)
                        {
                            tmpIndex = i;
                            break;
                        }
                    }

                    // Checking
                    if(tmpIndex < 0)
                    {
                        Console.WriteLine("No more entity for client available. Connection rejected!");
                        clientSock.Close();
                    }
                    else
                    {
                        // Set argument
                        mgrRef.mgrList[tmpIndex].client_task = mgrRef.client_task;
                        mgrRef.mgrList[tmpIndex].usrData = mgrRef.usrData;
                        mgrRef.mgrList[tmpIndex].cond = mgrRef.cond;

                        mgrRef.mgrList[tmpIndex].clientSock = clientSock;

                        // Create client thread
                        mgrRef.mgrList[tmpIndex].tHandle = new Thread(tcpmgr_client_thread);
                        mgrRef.mgrList[tmpIndex].tHandle.Start((object)mgrRef.mgrList[tmpIndex]);
                        mgrRef.mgrList[tmpIndex].occupied = 1;
                    }
                }
            }
        }

        private void tcpmgr_clean_task(object obj)
        {
            tcpmgr mgrRef = (tcpmgr)obj;

            Monitor.Enter(mgrRef.cond);

            while(mgrRef.stopFlag == 0)
            {
                // Wait condition
                Monitor.Wait(mgrRef.cond);

                // Join client thread
                for (int i = 0; i < mgrRef.mgrList.Length; i++)
                {
                    if (mgrRef.mgrList[i].closeJoin > 0)
                    {
                        mgrRef.mgrList[i].tHandle.Join();
                        mgrRef.mgrList[i].closeJoin = 0;
                        mgrRef.mgrList[i].occupied = 0;
                    }
                }
            }

            Monitor.Exit(mgrRef.cond);
        }
    }
}