﻿using System;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using System.Drawing;
using System.Text;
using System.Linq;
using System.Data;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;


namespace l1
{
    public partial class Lab4 : Form
    {
        public int thread_id = 1;
        public int modType = 0;

        [DllImport("..\\NamedPipes.dll", CharSet = CharSet.Ansi)]
        private static extern bool NPSend(int evType, int evId, StringBuilder msg);

        [DllImport("..\\NamedPipes.dll", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool NPStartThread(int evType);

        [DllImport("..\\NamedPipes.dll", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool NPStopThread(int evType);

        [DllImport("..\\NamedPipes.dll", CharSet = CharSet.Ansi)]
        private static extern int NPGetAmount(int evType);


        [DllImport("..\\WindowsSocket.dll", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool SocketSend(int evType, int evId, StringBuilder msg);

        [DllImport("..\\WindowsSocket.dll", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool SocketStartThread(int evType);

        [DllImport("..\\WindowsSocket.dll", CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool SocketStopThread(int evType);

        [DllImport("..\\WindowsSocket.dll", CharSet = CharSet.Ansi)]
        private static extern int SocketGetAmount(int evType);

        [DllImport("..\\WindowsSocket.dll", CharSet = CharSet.Ansi)]
        private static extern bool SocketSetMod(int modType);

        public bool StartThread(int mt)
        {
            if (mt == 1)
            {
                return SocketStartThread(0);
            }
            else
            {
                return NPStartThread(0);
            };
        }

        public bool StopThread(int mt)
        {
            if (mt == 1)
            {
                return SocketStopThread(1);
            }
            else
            {
                return NPStopThread(1);
            };
        }

        public bool SendMessage(int mt, int id, StringBuilder s)
        {
            StringBuilder text = new StringBuilder(s + "1111");
            if (mt == 1)
            {
                return SocketSend(2, id, text);
            }
            else
            {
                return NPSend(2, id, s);
            };
        }

        public Lab4() 
        {
            InitializeComponent(); 
            DllCh.Items.AddRange(new object[] { "NamedPipe", "Socket" });
            DllCh.SelectedItem = "Socket";
            UpdateInfo();
            
        }

        public void UpdateInfo() 
        {
            listBox1.Items.Clear();
            if (DllCh.SelectedItem.ToString() == "Socket")
                modType = 1;
            else  
                modType = 2; 

            var threads = modType == 1 ? SocketGetAmount(3) : NPGetAmount(3);
            listBox1.Items.Add("All Threads\n");
            listBox1.Items.Add("Main Thread\n");
            if (threads > 0)
            {
                for (int i = 1; i < threads; i++)
                {
                    listBox1.Items.Add("id " + i.ToString() + "\n");
                }
                thread_id = threads;
            }
        }

        private void Start_Click(object sender, EventArgs e)
        {

            UpdateInfo();
            if (thread_id == 1)
            {
                SocketSetMod(modType);
            }

            DllCh.Enabled = false;
            
            int thread_number = (int)thread_count.Value;
  
            for (int i = 0; i < thread_number; i++)
                {
                    if (StartThread(modType))
                    {
                        listBox1.Items.Add("id " + thread_id++.ToString() + "\n");
                        label1.Visible = false;
                        label1.Text = "";
                    }
                    else 
                    {
                        listBox1.Items.Clear();
                        label1.Visible = true; label1.Text = "No Available Server";
                        DllCh.Enabled = true;
                        thread_id = 1;
                    }
                }
        }

        private void Stop_click(object sender, EventArgs e)
        {
            UpdateInfo();
            
            StopThread(modType);
            if (thread_id > 1)
                listBox1.Items.RemoveAt(thread_id--);
            else
            {
                Stop.Enabled = false;
                listBox1.Items.Clear();
                thread_id = 1;
            }
        }


        private void SendButton_Click(object sender, EventArgs e)
        {
            StringBuilder fileText = new StringBuilder(textBox1.Text, Encoding.Unicode.GetBytes(textBox1.Text).Length);

            var index = (int)listBox1.SelectedIndex - 1;
            if (!(listBox1.SelectedIndex >= 0))
            {
                label1.Text = "Thread is not choosed\n";
                label1.Visible = true;
            }
            else
            {
                label1.Visible = false;
                if (listBox1.SelectedItem.ToString() == "All Threads\n")
                {
                    for (int i = 1; i < thread_id + 1; i++)
                    {
                        SendMessage(modType, i, fileText);
                    }
                }
                else if (listBox1.SelectedItem.ToString() == "Main Thread\n")
                {
                    SendMessage(modType, 0,  fileText);
                }
                else
                {
                    SendMessage(modType, index, fileText);
                }
            }
        }


    }
} 