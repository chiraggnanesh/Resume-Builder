#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#define MAX_EXP 10

typedef struct {
    char title[100];
    char company[100];
    char duration[50];
    char description[300];
} Experience;

Experience experiences[MAX_EXP];
int exp_count = 0;

/* Widgets */
GtkWidget *name_e, *phone_e, *email_e, *address_e;
GtkWidget *dob_e, *citizenship_e, *college_e, *degree_e, *skills_e;
GtkWidget *title_e, *company_e, *duration_e;
GtkWidget *desc_view, *exp_list;

/* ---------- Helper ---------- */
void add_row(GtkWidget *grid, const char *label, GtkWidget **entry, int row)
{
    GtkWidget *lbl = gtk_label_new(label);
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);

    *entry = gtk_entry_new();

    gtk_grid_attach(GTK_GRID(grid), lbl, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), *entry, 1, row, 1, 1);
}

/* ---------- Add Experience ---------- */
void add_experience(GtkWidget *w, gpointer d)
{
    if (exp_count >= MAX_EXP) return;

    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(desc_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buf, &start, &end);

    strcpy(experiences[exp_count].title,
           gtk_entry_get_text(GTK_ENTRY(title_e)));
    strcpy(experiences[exp_count].company,
           gtk_entry_get_text(GTK_ENTRY(company_e)));
    strcpy(experiences[exp_count].duration,
           gtk_entry_get_text(GTK_ENTRY(duration_e)));
    strcpy(experiences[exp_count].description,
           gtk_text_buffer_get_text(buf, &start, &end, FALSE));

    char preview[256];
    snprintf(preview, sizeof(preview), "%s  (%s)",
             experiences[exp_count].title,
             experiences[exp_count].duration);

    GtkWidget *row = gtk_label_new(preview);
    gtk_list_box_insert(GTK_LIST_BOX(exp_list), row, -1);
    gtk_widget_show_all(exp_list);

    exp_count++;

    gtk_entry_set_text(GTK_ENTRY(title_e), "");
    gtk_entry_set_text(GTK_ENTRY(company_e), "");
    gtk_entry_set_text(GTK_ENTRY(duration_e), "");
    gtk_text_buffer_set_text(buf, "", -1);
}

/* ---------- Generate CV ---------- */
void generate_cv(GtkWidget *w, gpointer d)
{
    const char *name = gtk_entry_get_text(GTK_ENTRY(name_e));
    if (strlen(name) == 0) return;

    char filename[150];
    snprintf(filename, sizeof(filename), "%s_cv.html", name);
    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    fprintf(fp, "<html><head><title>%s - CV</title><style>", name);
    fprintf(fp, "body{font-family:'Times New Roman';margin:50px 100px;}");
    fprintf(fp, ".center{text-align:center;}");
    fprintf(fp, ".section{font-weight:bold;margin-top:40px;}");
    fprintf(fp, "table{width:100%%;}");
    fprintf(fp, ".label{width:180px;font-weight:bold;}");
    fprintf(fp, "</style></head><body>");

    fprintf(fp, "<div class='center'><h3>CURRICULUM VITAE</h3><h1>%s</h1>", name);
    fprintf(fp, "%s<br>Telephone: %s<br>Email: %s</div>",
            gtk_entry_get_text(GTK_ENTRY(address_e)),
            gtk_entry_get_text(GTK_ENTRY(phone_e)),
            gtk_entry_get_text(GTK_ENTRY(email_e)));

    fprintf(fp, "<div class='section'>PERSONAL DATA</div><table>");
    fprintf(fp, "<tr><td class='label'>DOB</td><td>%s</td></tr>",
            gtk_entry_get_text(GTK_ENTRY(dob_e)));
    fprintf(fp, "<tr><td class='label'>Citizenship</td><td>%s</td></tr>",
            gtk_entry_get_text(GTK_ENTRY(citizenship_e)));
    fprintf(fp, "</table>");

    fprintf(fp, "<div class='section'>EDUCATION</div><table>");
    fprintf(fp, "<tr><td class='label'>Institution</td><td>%s</td></tr>",
            gtk_entry_get_text(GTK_ENTRY(college_e)));
    fprintf(fp, "<tr><td class='label'>Degree</td><td>%s</td></tr>",
            gtk_entry_get_text(GTK_ENTRY(degree_e)));
    fprintf(fp, "</table>");

    fprintf(fp, "<div class='section'>PROFESSIONAL EXPERIENCE</div><table>");
    for (int i = 0; i < exp_count; i++) {
        fprintf(fp, "<tr>");
        fprintf(fp, "<td class='label'>%s</td>", experiences[i].duration);
        fprintf(fp, "<td><b>%s</b><br><i>%s</i><br>%s</td>",
                experiences[i].title,
                experiences[i].company,
                experiences[i].description);
        fprintf(fp, "</tr><tr><td colspan='2'><br></td></tr>");
    }
    fprintf(fp, "</table>");

    fprintf(fp, "<div class='section'>SKILLS</div><li>%s</li>",
            gtk_entry_get_text(GTK_ENTRY(skills_e)));

    fprintf(fp, "</body></html>");
    fclose(fp);

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "CV Generated Successfully!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* ---------- MAIN ---------- */
int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Professional CV Builder");
    gtk_window_set_default_size(GTK_WINDOW(win), 900, 650);

    /* SPLIT PANE */
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_container_add(GTK_CONTAINER(win), paned);

    /* LEFT: FORM */
    GtkWidget *form_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_pack1(GTK_PANED(paned), form_scroll, TRUE, FALSE);

    GtkWidget *form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 15);
    gtk_container_add(GTK_CONTAINER(form_scroll), form_box);

    /* Personal */
    GtkWidget *p_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(p_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(p_grid), 15);

    add_row(p_grid, "Name", &name_e, 0);
    add_row(p_grid, "Phone", &phone_e, 1);
    add_row(p_grid, "Email", &email_e, 2);
    add_row(p_grid, "Address", &address_e, 3);
    add_row(p_grid, "DOB", &dob_e, 4);
    add_row(p_grid, "Citizenship", &citizenship_e, 5);

    GtkWidget *p_frame = gtk_frame_new("Personal Details");
    gtk_container_add(GTK_CONTAINER(p_frame), p_grid);
    gtk_box_pack_start(GTK_BOX(form_box), p_frame, FALSE, FALSE, 0);

    /* Education */
    GtkWidget *e_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(e_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(e_grid), 15);

    add_row(e_grid, "College", &college_e, 0);
    add_row(e_grid, "Degree", &degree_e, 1);
    add_row(e_grid, "Skills", &skills_e, 2);

    GtkWidget *e_frame = gtk_frame_new("Education & Skills");
    gtk_container_add(GTK_CONTAINER(e_frame), e_grid);
    gtk_box_pack_start(GTK_BOX(form_box), e_frame, FALSE, FALSE, 0);

    /* Experience */
    GtkWidget *x_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(x_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(x_grid), 15);

    add_row(x_grid, "Job Title", &title_e, 0);
    add_row(x_grid, "Company", &company_e, 1);
    add_row(x_grid, "Duration", &duration_e, 2);

    desc_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(desc_view), GTK_WRAP_WORD);
    gtk_widget_set_size_request(desc_view, -1, 80);
    gtk_grid_attach(GTK_GRID(x_grid), gtk_label_new("Description"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(x_grid), desc_view, 1, 3, 1, 1);

    GtkWidget *add_btn = gtk_button_new_with_label("Add Experience");
    gtk_grid_attach(GTK_GRID(x_grid), add_btn, 1, 4, 1, 1);

    GtkWidget *x_frame = gtk_frame_new("Professional Experience");
    gtk_container_add(GTK_CONTAINER(x_frame), x_grid);
    gtk_box_pack_start(GTK_BOX(form_box), x_frame, FALSE, FALSE, 0);

    GtkWidget *gen_btn = gtk_button_new_with_label("Generate CV");
    gtk_box_pack_start(GTK_BOX(form_box), gen_btn, FALSE, FALSE, 10);

    /* RIGHT: PREVIEW */
    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(right_box), 15);
    gtk_paned_pack2(GTK_PANED(paned), right_box, TRUE, FALSE);

    GtkWidget *info = gtk_label_new(
        "Experience Preview\n\n"
        "• Click 'Add Experience' to store entries\n"
        "• Multiple experiences supported\n"
        "• Generate CV when finished"
    );
    gtk_label_set_xalign(GTK_LABEL(info), 0);
    gtk_box_pack_start(GTK_BOX(right_box), info, FALSE, FALSE, 0);

    exp_list = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(right_box), exp_list, TRUE, TRUE, 0);

    /* Signals */
    g_signal_connect(add_btn, "clicked", G_CALLBACK(add_experience), NULL);
    g_signal_connect(gen_btn, "clicked", G_CALLBACK(generate_cv), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}
