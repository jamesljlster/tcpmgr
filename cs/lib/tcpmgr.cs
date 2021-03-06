﻿using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace netlib
{
    public partial class tcpmgr
    {
        // Private data types
        public delegate void tcpmgr_client_task(object usrData, Socket sock);

        private class tcpmgr_list
        {
            public Thread tHandle;
            public object cond;

            public int sockStatus;
            public Socket clientSock;

            public int occupied;
            public int closeJoin;

            public tcpmgr_client_task client_task;
            public object usrData;
        }

        // Constructor
        public tcpmgr(String hostIP, int hostPort, int maxClient)
        {
            // Check argument
            if (maxClient <= 0)
            {
                throw new ArgumentException("Invalid maxClient argument");
            }

            // Create client manage list
            this.mgrList = new tcpmgr_list[maxClient];
            for (int i = 0; i < maxClient; i++)
            {
                this.mgrList[i] = new tcpmgr_list();
            }

            // Bind and listen
            this.listenSock = new TcpListener(IPAddress.Parse(hostIP), hostPort);
            this.listenSock.Start(maxClient);
        }

        // Destructor
        ~tcpmgr()
        {
            this.listenSock.Stop();
            this.stop();
        }

        // Private objects
        int stopFlag;
        tcpmgr_list[] mgrList;

        object cond = new object();

        Thread cleanTask;
        Thread acceptTask;

        TcpListener listenSock;

        tcpmgr_client_task client_task;
        object usrData;

        public void start(tcpmgr_client_task client_task, object arg)
        {
            this.client_task = client_task;
            this.usrData = arg;

            // Create clean task
            this.cleanTask = new Thread(tcpmgr_clean_task);
            this.cleanTask.Start(this);

            // Create accept task
            this.acceptTask = new Thread(tcpmgr_accept_thread);
            this.acceptTask.Start(this);
        }

        public void stop()
        {
            // Cancel and join accept task
            if (this.acceptTask != null)
            {
                if (this.acceptTask.IsAlive)
                {
                    this.acceptTask.Abort();
                }
            }

            // Cancel clean task
            if (this.cleanTask != null)
            {
                if (this.cleanTask.IsAlive)
                {
                    this.cleanTask.Abort();
                }
            }

            // Cancel all client task
            this.server_cleanup();
        }

        private void server_cleanup()
        {
            for (int i = 0; i < this.mgrList.Length; i++)
            {
                if (this.mgrList[i].occupied > 0)
                {
                    this.mgrList[i].tHandle.Abort();
                    this.mgrList[i].closeJoin = 1;
                }

                if (this.mgrList[i].closeJoin > 0)
                {
                    this.mgrList[i].tHandle.Join();
                    this.mgrList[i].closeJoin = 0;
                    this.mgrList[i].occupied = 0;
                }

                if (this.mgrList[i].clientSock != null)
                {
                    if (this.mgrList[i].clientSock.Connected)
                    {
                        this.mgrList[i].clientSock.Close();
                    }
                }
            }
        }
    }
}
