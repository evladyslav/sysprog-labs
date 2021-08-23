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
    public partial class Form1 : Form
    {
        public int thread_id = 1;
        //Process Child = null;

        bool Child = false;

        EventWaitHandle evStart = new EventWaitHandle(false, EventResetMode.ManualReset, "EventStart");
        EventWaitHandle evStop = new EventWaitHandle(false, EventResetMode.AutoReset, "EventStop");
        EventWaitHandle evSubmit = new EventWaitHandle(false, EventResetMode.ManualReset, "ThreadSubmit");

        [DllImport("..\\AnonymousDll.dll", CharSet = CharSet.Ansi)]
        private static extern void Send(StringBuilder Str);

        [DllImport("..\\AnonymousDll.dll", CharSet = CharSet.Ansi)]
        public static extern void StartChild();

        [DllImport("..\\AnonymousDll.dll", CharSet = CharSet.Ansi)]
        public static extern int KillChild();
        public Form1() { InitializeComponent(); }

        //private void ChildExited(object sender, EventArgs e)
        //{
        //    if (InvokeRequired)
        //        this.Invoke(new Action(() => listBox1.Items.Clear()));
        //    else
        //        listBox1.Items.Clear();
        //    thread_id = 1;
        //}

        private void Start_Click(object sender, EventArgs e)
        {
            int thread_number = (int)thread_count.Value;
            if (Child == false)
            {
                StartChild(); 
                listBox1.Items.Add("All Threads\n");
                listBox1.Items.Add("Main Thread\n");
                Child = true;
            }
            else
            {
                for (int i = 0; i < thread_number; i++)
                {
                    evStart.Set();
                    if (evSubmit.WaitOne(10000))
                    {
                        listBox1.Items.Add("id " + thread_id++.ToString() + "\n");
                        evSubmit.Reset();
                    }
                    else
                        MessageBox.Show("Smth wrong...");
                }
            }
        }

        private void Stop_click(object sender, EventArgs e)
        {
            if (!(Child == true))
                return;
            evStop.Set();
            if (evSubmit.WaitOne(1000))
            {
                if (thread_id > 1)
                    listBox1.Items.RemoveAt(thread_id--); 
                else
                {
                    Child = false; 
                    listBox1.Items.Clear();
                }
                evSubmit.Reset();
            }
            else
                MessageBox.Show("Smth wrong...While stopping");
        }


        private void SendButton_Click(object sender, EventArgs e)
        {
            StringBuilder fileText = new StringBuilder(textBox1.Text, Encoding.Unicode.GetBytes(textBox1.Text).Length);
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
                        //var index = (int)listBox1.SelectedIndex - 1;
                        var msgText = "NewMessage" + i;
                        EventWaitHandle Message = new EventWaitHandle(false, EventResetMode.ManualReset, msgText);
                        Send(fileText);
                        Message.Set();
                    }
                }
                else if (listBox1.SelectedItem.ToString() == "Main Thread\n")
                {
                    EventWaitHandle Message = new EventWaitHandle(false, EventResetMode.ManualReset, "evMain");
                    Send(fileText);
                    Message.Set();
                }
                else
                {
                    var index = (int)listBox1.SelectedIndex - 1;
                    var msgText = "NewMessage" + index;
                    EventWaitHandle Message = new EventWaitHandle(false, EventResetMode.ManualReset, msgText);
                    Send(fileText);
                    Message.Set();
                }
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {

            EventWaitHandle Message = new EventWaitHandle(false, EventResetMode.ManualReset, "CloseProgram");
            Message.Set();
        }
    }
} 