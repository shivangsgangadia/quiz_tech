#include <gtk/gtk.h>
#include <glib.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include "utilities.cpp"
#include "testingclient.cpp"
#include <iostream>

using namespace std;

const char* yellow = "#ededd4d40000";
const char* red  = "#cccc00000000";
const char* green = "#7373d2d21616";


//Global variables

//Global widgets for main window
GtkBuilder *builder; 
GtkWidget *window_main;		//GtkWindow
GtkButton *submit_button;	//GtkButton
GtkTextView *question_text;	//GtkTextView
GtkTextBuffer *question_text_buffer;	//GtkTextBuffer
GtkEntry *answer_box;		//GtkEntry
GtkRadioButton *q_button_array[20];	//GtkToggleButton
GtkLabel *question_title_label;	//GtkLabel
GtkLabel *attempts_left_label;		//GtkLabel
GtkLabel *serverip_label;		//GtkLabel
GtkLabel *submission_status_label;		//GtkLabel
GtkLabel *teamid_label;		//GtkLabel
GtkLabel *difficulty_label;	//GtkLabel
GtkLabel *points_count;		//GtkLabel


//Global widgets for login window
GtkWidget *login_window;	//GtkWindow
GtkEntry *team_id_entry;	//GtkEntry
GtkEntry *server_ip_entry;	//GtkEntry
GtkLabel *team_id_status_label;	//GtkLabel
GtkLabel *server_ip_status_label; //GtkLabel
GtkButton *login_button;	//GtkButton


//Widgets for error dialogs
GtkWidget *conn_error_dialog;
GtkWidget *data_unavail_dialog;

//Constant data about the user
string teamID;
int current_question;
int points;

int *argc_univ; 
char ***argv_univ;


static void gtk_set_label_color(GtkWidget* label, const char* color)
{
	static GtkCssProvider* provider = NULL;
    const char* format = "label { color: %s; }";
    size_t length = strlen(format) - 2 + 1;
    char style[length];
    sprintf(style, format, color);

    if (provider == NULL) {
        /* only create and add the provider the first time */
        provider = gtk_css_provider_new();
        gtk_style_context_add_provider(
            gtk_widget_get_style_context(label),
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        g_object_unref(provider);
    }

    gtk_css_provider_load_from_data(provider, style, -1, NULL);
}



//This function loads the initial GUI
void setup_main_window()
{
	void get_and_set_question(GtkWidget*,gpointer);
	void submit_answer();
	points = 0;
	
    gtk_init(argc_univ, argv_univ);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "gui.glade", NULL);

	//Getting widgets from glade file builder
    window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    submit_button = GTK_BUTTON(gtk_builder_get_object(builder, "submit_button"));
    question_text_buffer = gtk_text_buffer_new(NULL);
    question_text = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "question_text"));
    gtk_text_view_set_buffer(question_text, question_text_buffer);
    gtk_text_view_set_editable(question_text, FALSE);
    answer_box = GTK_ENTRY(gtk_builder_get_object(builder, "answer_box"));
    question_title_label = GTK_LABEL(gtk_builder_get_object(builder, "question_title_label"));
    attempts_left_label = GTK_LABEL(gtk_builder_get_object(builder, "attempts_left_label"));
    serverip_label = GTK_LABEL(gtk_builder_get_object(builder, "serverip_label"));
    submission_status_label = GTK_LABEL(gtk_builder_get_object(builder, "submission_status_label"));
    teamid_label = GTK_LABEL(gtk_builder_get_object(builder, "teamid_label"));
    difficulty_label = GTK_LABEL(gtk_builder_get_object(builder, "difficulty_label"));
    points_count = GTK_LABEL(gtk_builder_get_object(builder, "points_count"));
   
    //q_number_list++;
    for(int i = 1; i<=20; i++)
    {
		//cout<<"\n"<<q_button_generator(i).c_str();
		q_button_array[i-1] = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, q_button_generator(i).c_str()));
		g_signal_connect(G_OBJECT(q_button_array[i-1]), "clicked",G_CALLBACK(get_and_set_question), GINT_TO_POINTER(i));	
	}
	gtk_label_set_text(difficulty_label, "");
	
	//Setting up all widgets to initial values
    g_signal_connect(GTK_WIDGET(window_main), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(GTK_WIDGET(submit_button), "clicked", G_CALLBACK(submit_answer), NULL);
    g_object_unref(builder);
 
    gtk_widget_show_all(window_main);                
    gtk_main();
}


void setup_login_window()
{
	void get_and_set_question(GtkWidget* , gpointer);
	void try_login();
	gtk_init(argc_univ, argv_univ);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "login.glade", NULL);
    
    //Getting widgets from builder
    login_window = GTK_WIDGET(gtk_builder_get_object(builder, "login_window"));
    team_id_entry = GTK_ENTRY(gtk_builder_get_object(builder, "team_id_entry"));
    server_ip_entry = GTK_ENTRY(gtk_builder_get_object(builder, "server_ip_entry"));
    team_id_status_label = GTK_LABEL(gtk_builder_get_object(builder, "team_id_status_label"));
    server_ip_status_label = GTK_LABEL(gtk_builder_get_object(builder, "server_ip_status_label"));
    login_button = GTK_BUTTON(gtk_builder_get_object(builder, "login_button"));
    
    
    //Signal connects
    g_signal_connect(G_OBJECT(login_button), "clicked",G_CALLBACK(try_login), NULL);
    g_signal_connect(G_OBJECT(login_button), "clicked",G_CALLBACK(get_and_set_question), GINT_TO_POINTER(1));
    g_signal_connect(GTK_WIDGET(login_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    
    g_object_unref(builder);
    gtk_widget_show_all(login_window);                
    gtk_main();
}

void setup_conn_dialog()
{
	gtk_init(argc_univ, argv_univ);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "error_conn.glade", NULL);
    //Getting widgets from builder
    conn_error_dialog = GTK_WIDGET(gtk_builder_get_object(builder, "conn_error_dialog"));
    g_signal_connect(GTK_WIDGET(conn_error_dialog), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    g_object_unref(builder);
    gtk_dialog_run(GTK_DIALOG(conn_error_dialog));                
    gtk_main();
}

void setup_data_unavail_dialog()
{
	gtk_init(argc_univ, argv_univ);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "data_unavail.glade", NULL);
    //Getting widgets from builder
    data_unavail_dialog = GTK_WIDGET(gtk_builder_get_object(builder, "data_unavail_dialog"));
    g_signal_connect(GTK_WIDGET(data_unavail_dialog), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    
    g_object_unref(builder);
    gtk_dialog_run(GTK_DIALOG(data_unavail_dialog));                
    gtk_main();
}

void try_login()
{
	void get_and_set_question(GtkWidget*,gpointer);
	string team_id = gtk_entry_get_text (team_id_entry);
	string server_ip = gtk_entry_get_text (server_ip_entry);
	vector<string> to_send(0);
	string temp = "l";
	to_send.push_back(temp.c_str());
	to_send.push_back(team_id);
	string data = create_send_data(to_send);
	set_server(server_ip);
	try
	{
		string response = exchange_data(data);		//get response
		vector<string> r = stringSplit(response, ';');
		//string succ = "success";
		if(r[0]=="S")
		{
			teamID = team_id;
			gtk_window_close(GTK_WINDOW(login_window));
			setup_main_window();
			gtk_label_set_text(teamid_label, teamID.c_str());
			gtk_label_set_text(serverip_label, server_ip.c_str());
			gtk_label_set_text(points_count, (r[1]).c_str());
			string re = "q;1;";
			re.append(teamID);
			string res = exchange_data(re);
			vector<string> resp_array = stringSplit(res, ';');
			string ques_title = resp_array[0];
			string ques_text = resp_array[1];
			string difficulty = resp_array[2];
			string attempts_left = resp_array[3];
			gtk_label_set_text(question_title_label, ques_title.c_str());
			gtk_label_set_text(attempts_left_label, attempts_left.c_str());
			if(difficulty[0]=='e')	gtk_label_set_text(difficulty_label, "Easy");
			else if(difficulty[0]=='m')	gtk_label_set_text(difficulty_label, "Medium");
			else if(difficulty[0]=='h')	gtk_label_set_text(difficulty_label, "Hard");
			if(difficulty[0]=='b')	gtk_label_set_text(difficulty_label, "Bonus");
			gtk_text_buffer_set_text(question_text_buffer, ques_text.c_str(), -1);
		}
		else if(r[0]=="N")
		{
			gtk_entry_set_text(team_id_entry, "");
			gtk_entry_set_text(server_ip_entry, "");
			gtk_label_set_text(team_id_status_label, "Team Not Registered!");
			gtk_set_label_color(GTK_WIDGET(team_id_status_label), red);
			//gtk_label_set_text(server_ip_status_label, "Or maybe this is wrong..Try both again");
			gtk_set_label_color(GTK_WIDGET(server_ip_status_label), red);
		}
		else if(r[0]=="F")
		{
			gtk_entry_set_text(team_id_entry, "");
			gtk_entry_set_text(server_ip_entry, "");
			gtk_label_set_text(team_id_status_label, "Server logic issue...contact Admin");
			gtk_set_label_color(GTK_WIDGET(team_id_status_label), red);
			//gtk_label_set_text(server_ip_status_label, "Or maybe this is wrong..Try both again");
			gtk_set_label_color(GTK_WIDGET(server_ip_status_label), red);
		}
	}
	catch(std::out_of_range &e)
	{
		gtk_window_set_transient_for(GTK_WINDOW(data_unavail_dialog), GTK_WINDOW(login_window));
		setup_data_unavail_dialog();
	}
	catch(Poco::Net::ConnectionRefusedException &ex)
    {
		gtk_window_set_transient_for(GTK_WINDOW(conn_error_dialog), GTK_WINDOW(login_window));
		setup_conn_dialog();
	}
	
}
//Function to submit answer
void submit_answer()
{
	string answer = gtk_entry_get_text (answer_box);
	gtk_entry_set_text (answer_box, "");
	string request = "a;";
	request.append(teamID);
	request.append(";");
	request.append(answer);
	try
	{
		string response = exchange_data(request);
		vector<string> resp_array = stringSplit(response, ';');
		string result = resp_array[0];
		if(result[0]=='c')
		{
			points += to_int(resp_array[1]);
			gtk_label_set_text(points_count, (to_string(points)).c_str());
			gtk_label_set_text(attempts_left_label, "0");
			gtk_label_set_text(submission_status_label, "Correct answer!");
		}
		else if(result[0]=='x')
		{
			gtk_label_set_text(submission_status_label, "Already answered or attempts over");
		}
		else if(result[0]=='w')
		{
			gtk_label_set_text(submission_status_label, "Sorry, wrong answer...");
			gtk_label_set_text(attempts_left_label, resp_array[1].c_str());
		}
	}
	catch(std::out_of_range &e)
	{
		gtk_window_set_transient_for(GTK_WINDOW(data_unavail_dialog), GTK_WINDOW(window_main));
		setup_data_unavail_dialog();
	}
	catch(Poco::Net::ConnectionRefusedException &ex)
    {
		gtk_window_set_transient_for(GTK_WINDOW(conn_error_dialog), GTK_WINDOW(window_main));
		setup_conn_dialog();
	}	
}

//Function to get question from the server and set it into the UI
void get_and_set_question(GtkWidget *wid, gpointer q_no)
{
	int q = GPOINTER_TO_INT(q_no);
	cout<<"\nFor testing value of q: "<<q;
	current_question = q;
	string request = "q;";
	request.append(to_string(q));
	request.append(";");
	request.append(teamID);
	cout<<"\nFor testing value of query: "<<request;
	try
	{
		string response = exchange_data(request);
		vector<string> resp_array = stringSplit(response, ';');
		string ques_title = resp_array[0];
		string ques_text = resp_array[1];
		string difficulty = resp_array[2];
		string attempts_left = resp_array[3];
		gtk_label_set_text(question_title_label, ques_title.c_str());
		gtk_label_set_text(attempts_left_label, attempts_left.c_str());
		if(difficulty[0]=='e')	gtk_label_set_text(difficulty_label, "Easy");
		else if(difficulty[0]=='m')	gtk_label_set_text(difficulty_label, "Medium");
		else if(difficulty[0]=='h')	gtk_label_set_text(difficulty_label, "Hard");
		if(difficulty[0]=='b')	gtk_label_set_text(difficulty_label, "Bonus");
		gtk_text_buffer_set_text(question_text_buffer, ques_text.c_str(), -1);
	}
	catch(std::out_of_range &e)
	{
		setup_data_unavail_dialog();
	}
}
	



int main(int argc, char **argv)
{
	argc_univ = &argc;
	argv_univ = &argv;
	setup_login_window();
	//setup_main_window();
}
