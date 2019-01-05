<div class="tabs">
  <!-- Print tabs with flea-market names -->
  <?php $is_this_first_tab = TRUE;
  foreach ($output_containers as $value) {
    $current_tab_title = $value['container_title'];
    $current_tab_id = $value['market_id'].'_tab' ?>

    <input id="<?php print $current_tab_id ?>" type="radio" name="tabs" <?php $is_this_first_tab ? print 'checked' : print '';  ?>>
    <label for="<?php print $current_tab_id ?>" title="<?php print $current_tab_title ?>"><?php print $current_tab_title ?></label>
    <?php $is_this_first_tab = FALSE; } unset($value); ?>

  <!-- Print content sections corresponded to each tab -->
  <?php foreach ($output_containers as $value) {
    $current_content_section_id = $value['container_id'] ?>
    <section id="<?php print $current_content_section_id ?>">
      <p>
        Здесь текст
      </p>
    </section>
  <?php } unset($value); ?>
</div>
