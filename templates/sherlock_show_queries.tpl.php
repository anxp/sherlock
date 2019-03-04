<div class="sherlock-show-queries-parentblock">
    <div id="sherlock_show_queries_header" class="sherlock-show-queries-header">
        <?php print '<h3>'.t('List of constructed search queries (click to show).').'</h3>' ?>
    </div>
    <div id="sherlock_show_queries_contentarea" class="sherlock-show-queries-contentarea">
        <ul>
          <?php foreach ($constructed_queries as $key => $value) { ?>
              <li><?php print $key; ?>
                  <ul>
                    <?php foreach ($value as $v) { ?>
                        <li>
                          <?php print $v; ?>
                        </li>
                    <?php } unset($v); ?>
                  </ul>
              </li>
          <?php } unset($key, $value); ?>
        </ul>
    </div>
</div>
